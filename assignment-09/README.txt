# To make it i checked the source code for /proc/mounts: (base.c/mountstats_open) https://elixir.bootlin.com/linux/v4.9/source/fs/proc_namespace.c#L235

# current is a pointer to the current process (i.e. the process that issued the system call): https://elixir.bootlin.com/linux/latest/source/arch/microblaze/include/asm/current.h#L23
[...]
static __always_inline struct task_struct *get_current(void)
{
    return percpu_read_stable(current_task);
}
#define current get_current()
[...]

# From current (struct task_struct) we can reach nsproxy:
# A structure that contains pointers to all per-process namespaces - fs (mount), uts, network, sysvipc, etc.
# The list of available mount namespaces is available through nsproxy->mnt_ns
nsp = current->nsproxy;
mnt_ns = nsp->mnt_ns;

# To parse the list, we must use list_for_each_entry(mnt_current, &mnt_ns->list, mnt_list)
# This way we iterate through our mounts

# I collect data for each entry then i store informations in buffer rbuf.
# I return it when the user is reading my /proc entry

========================================Filesystem===============================================================

path objects: describes the path under a file system, which contains the vfsmount structure of its file system and the dentry of the node

dentry objects: representing a translation from a path to an inode

inode objects: used to represent files and directories on the file systems

file objects: points to a dentry, which points to an inode that represents the actual file
	      contains information such as the "current position"
	      contains a pointer to 'file operations'

Each inode is potentially linked with a dentry

========================================Namespaces===============================================================

Namespaces provide processes with their own system view, thus isolating independent processes from each other.
In other words, namespaces define the set of resources that a process can use.

============Types of namespaces
In the current stable Linux Kernel version 5.7 there are seven different namespaces:

PID namespace: isolation of the system process tree;
NET namespace: isolation of the host network stack;
MNT namespace: isolation of host filesystem mount points;
UTS namespace: isolation of hostname;
IPC namespace: isolation for interprocess communication utilities (shared segments, semaphores);
USER namespace: isolation of system users IDs;
CGROUP namespace: isolation of the virtual cgroup filesystem of the host.

============Mount: https://medium.com/geekculture/linux-namespaces-container-technology-a09da0813247

On Linux, the whole directory tree structure is determined by the mount table. The mount table tells us: what filesystem is mounted to what mount point.
By default, Linux creates one init mount namespace. The root mount is contained inside this namespace

The mounts in Linux are in a tree structure. The parent mount can have multiple children.

When Linux searches for a filename, it starts from the root file system current->nsproxy->mnt_namespace->root.

unshare command can be used to create a new mount namespace. When the new mount namespace is created, it receives a copy of the mounts from the parent namespace. All the child processes in the new mnt-namespace see no difference in the directory tree. Their view starts to change when the child processes do new mounts in the new mnt-namespace.

==Experiment
~$ unshare -Umr
~# mount -t tmpfs none /home/ryan/mnt
~/mnt# mkdir test
 ~/mnt# ls
test
### start a new tab, check whether test folder exist
~/mnt$ ls
In the first tab, ls shows test folder. However, in the second tab, ls shows nothing. This is because unshare -m creates a new mount namespace, and starts a shell process in the new mnt-namespace. The mounts in the child mnt-namespace is not visible to the parent by default.

=========================================Structure declarations===========================================================

===========IN CODE DEFINITION
The struct in fs/mount.h is not exported. You would simply have to do the same in your code: partially redefine struct mnt_namespace.

===========NS_PROXY: https://elixir.bootlin.com/linux/latest/source/include/linux/nsproxy.h#L31
struct nsproxy {
	atomic_t count;
	struct uts_namespace *uts_ns;
	struct ipc_namespace *ipc_ns;
	struct mnt_namespace *mnt_ns;
	struct pid_namespace *pid_ns_for_children;
	struct net 	     *net_ns;
	struct time_namespace *time_ns;
	struct time_namespace *time_ns_for_children;
	struct cgroup_namespace *cgroup_ns;
};
===========MNT_NAMESPACE: https://elixir.bootlin.com/linux/latest/source/fs/mount.h#L8
struct mnt_namespace {
	struct ns_common		ns;
	struct mount			*root;
	struct list_head		list;
	spinlock_t				ns_lock;
	struct user_namespace	*user_ns;
	struct ucounts			*ucounts;
	u64						seq;	/* Sequence number to prevent loops */
	wait_queue_head_t		poll;
	u64						event;
	unsigned int			mounts; /* # of mounts in the namespace */
	unsigned int			pending_mounts;
} __randomize_layout;

===========MOUNT: https://elixir.bootlin.com/linux/latest/source/fs/mount.h#L39
struct mount {
	struct hlist_node mnt_hash;
	struct mount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct vfsmount mnt;
	union {
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;	/* list of children, anchored here */
	struct list_head mnt_child;	/* and going through their mnt_child */
	struct list_head mnt_instance;	/* mount instance on sb->s_mounts */
	const char *mnt_devname;	/* Name of device e.g. /dev/dsk/hda1 */
	struct list_head mnt_list;
	struct list_head mnt_expire;	/* link in fs-specific expiry list */
	struct list_head mnt_share;	/* circular list of shared mounts */
	struct list_head mnt_slave_list;/* list of slave mounts */
	struct list_head mnt_slave;	/* slave list entry */
	struct mount *mnt_master;	/* slave is on master->mnt_slave_list */
	struct mnt_namespace *mnt_ns;	/* containing namespace */
	struct mountpoint *mnt_mp;	/* where is it mounted */
	union {
		struct hlist_node mnt_mp_list;	/* list mounts with the same mountpoint */
		struct hlist_node mnt_umount;
	};
	struct list_head mnt_umounting; /* list entry for umount propagation */
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
#endif
	int mnt_id;			/* mount identifier */
	int mnt_group_id;		/* peer group identifier */
	int mnt_expiry_mark;		/* true if marked for expiry */
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
} __randomize_layout;

=========== TASK_STRUCT: https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h#L726
struct task_struct {
	[...]
	struct fs_struct *fs; /* Filesystem information: */
	struct nsproxy *nsproxy; /* Namespaces */
	[...]
}

=========== PATH: https://elixir.bootlin.com/linux/latest/source/include/linux/path.h#L8
struct path {
	struct vfsmount *mnt;
	struct dentry *dentry;
} __randomize_layout;

=========== DENTRY: https://elixir.bootlin.com/linux/latest/source/include/linux/dcache.h#L81
struct dentry {
	/* RCU lookup touched fields */
	unsigned int d_flags;		/* protected by d_lock */
	seqcount_spinlock_t d_seq;	/* per dentry seqlock */
	struct hlist_bl_node d_hash;	/* lookup hash list */
	struct dentry *d_parent;	/* parent directory */
	struct qstr d_name;
	struct inode *d_inode;		/* Where the name belongs to - NULL is
					 * negative */
	unsigned char d_iname[DNAME_INLINE_LEN];	/* small names */

	/* Ref lookup also touches following */
	struct lockref d_lockref;	/* per-dentry lock and refcount */
	const struct dentry_operations *d_op;
	struct super_block *d_sb;	/* The root of the dentry tree */
	unsigned long d_time;		/* used by d_revalidate */
	void *d_fsdata;			/* fs-specific data */

	union {
		struct list_head d_lru;		/* LRU list */
		wait_queue_head_t *d_wait;	/* in-lookup ones only */
	};
	struct list_head d_child;	/* child of parent list */
	struct list_head d_subdirs;	/* our children */
	/*
	 * d_alias and d_rcu can share memory
	 */
	union {
		struct hlist_node d_alias;	/* inode alias list */
		struct hlist_bl_node d_in_lookup_hash;	/* only for in-lookup ones */
	 	struct rcu_head d_rcu;
	} d_u;
} __randomize_layout;
