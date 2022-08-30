// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/nsproxy.h>
#include <linux/proc_fs.h>
#include <linux/mnt_namespace.h>
#include <linux/mount.h>
#include <linux/ns_common.h>
#include <linux/fs_struct.h>
#include <linux/uaccess.h>

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

struct mnt_namespace {
	struct ns_common	ns;
	struct mount *	root;
	/*
	 * Traversal and modification of .list is protected by either
	 * - taking namespace_sem for write, OR
	 * - taking namespace_sem for read AND taking .ns_lock.
	 */
	struct list_head	list;
	spinlock_t		ns_lock;
	struct user_namespace	*user_ns;
	struct ucounts		*ucounts;
	u64			seq;	/* Sequence number to prevent loops */
	wait_queue_head_t poll;
	u64 event;
	unsigned int		mounts; /* # of mounts in the namespace */
	unsigned int		pending_mounts;
} __randomize_layout;

#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME		"mymounts"

static struct proc_dir_entry *proc_file;

static char procfs_buffer[PROCFS_MAX_SIZE];

static unsigned long procfs_buffer_size = 0;

static ssize_t procfile_read(struct file *file, char __user *user_buffer,
		      size_t size, loff_t *offset)
{
	int ret = 1;
	if (*offset > 0)
		return 0;
	ret = copy_to_user(user_buffer, procfs_buffer, procfs_buffer_size);
	*offset += procfs_buffer_size - ret;
	return procfs_buffer_size - ret;
}

static int procfile_open(struct inode *inode, struct file *file)
{
	struct nsproxy *nsp;
	struct mnt_namespace *ns;
	struct mount *mnt_root;
	struct path path_root;

	pr_info("[*] Open procfile\n");

	nsp = current->nsproxy;
	ns = nsp->mnt_ns;

	return 0;
}

static int procfile_close(struct inode *inodep, struct file *filep)
{
	pr_info("[*] Close procfile\n");
	return 0;
}

static const struct file_operations proc_file_fops = {
	.owner = THIS_MODULE,
	.open = procfile_open,
	.read = procfile_read,
	.release = procfile_close
};

static int proc_init(void)
{
	printk(KERN_INFO "[*] Proc Module Constructor\n");
	proc_file = proc_create(PROCFS_NAME, 0664, NULL, &proc_file_fops);
	if (proc_file == NULL)
		return -ENOMEM;
	return 0;
}

static void proc_exit(void)
{
	printk(KERN_INFO "[*] Proc Module Destructor\n");
	proc_remove(proc_file);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mymounts module");
MODULE_AUTHOR("lumenthi");
