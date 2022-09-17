// SPDX-License-Identifier: GPL-2.0+

#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/ns_common.h>
#include <linux/fs_pin.h>

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
};

struct mnt_namespace {
	atomic_t				count;
	struct ns_common		ns;
	struct mount			*root;
	struct list_head		list;
	struct user_namespace	*user_ns;
	struct ucounts			*ucounts;
	u64						seq;	/* Sequence number to prevent loops */
	wait_queue_head_t		poll;
	u64						event;
	unsigned int			mounts; /* # of mounts in the namespace */
	unsigned int			pending_mounts;
};

struct fs_struct {
	int			users;
	spinlock_t	lock;
	seqcount_t	seq;
	int			umask;
	int			in_exec;
	struct path root, pwd;
};

struct nsproxy {
	atomic_t count;
	struct uts_namespace	*uts_ns;
	struct ipc_namespace	*ipc_ns;
	struct mnt_namespace	*mnt_ns;
	struct pid_namespace	*pid_ns_for_children;
	struct net				*net_ns;
	struct cgroup_namespace	*cgroup_ns;
};

static inline void get_fs_root(struct fs_struct *fs, struct path *root)
{
	spin_lock(&fs->lock);
	*root = fs->root;
	path_get(root);
	spin_unlock(&fs->lock);
}

static inline void get_mnt_ns(struct mnt_namespace *ns)
{
	atomic_inc(&ns->count);
}

static int prepend_name(char **buffer, int *buflen, const struct qstr *name)
{
	const char *dname = READ_ONCE(name->name);
	u32 dlen = READ_ONCE(name->len);
	char *p;

	smp_read_barrier_depends();

	*buflen -= dlen + 1;
	if (*buflen < 0)
		return -ENAMETOOLONG;
	p = *buffer -= dlen + 1;
	*p++ = '/';
	while (dlen--) {
		char c = *dname++;
		if (!c)
			break;
		*p++ = c;
	}
	return 0;
}

static int prepend(char **buffer, int *buflen, const char *str, int namelen)
{
	*buflen -= namelen;
	if (*buflen < 0)
		return -ENAMETOOLONG;
	*buffer -= namelen;
	memcpy(*buffer, str, namelen);
	return 0;
}

static char *__dentry_path(struct dentry *d, char *buf, int buflen)
{
	struct dentry *dentry;
	char *end, *retval;
	int len, seq = 0;
	int error = 0;

	if (buflen < 2)
		goto Elong;

	rcu_read_lock();
restart:
	dentry = d;
	end = buf + buflen;
	len = buflen;
	prepend(&end, &len, "\0", 1);
	/* Get '/' right */
	retval = end-1;
	*retval = '/';
	read_seqbegin_or_lock(&rename_lock, &seq);
	while (!IS_ROOT(dentry)) {
		struct dentry *parent = dentry->d_parent;

		prefetch(parent);
		error = prepend_name(&end, &len, &dentry->d_name);
		if (error)
			break;

		retval = end;
		dentry = parent;
	}
	if (!(seq & 1))
		rcu_read_unlock();
	if (need_seqretry(&rename_lock, seq)) {
		seq = 1;
		goto restart;
	}
	done_seqretry(&rename_lock, seq);
	if (error)
		goto Elong;
	return retval;
Elong:
	return ERR_PTR(-ENAMETOOLONG);
}

char *dentry_path(struct dentry *dentry, char *buf, int buflen)
{
	char *p = NULL;
	char *retval;

	if (d_unlinked(dentry)) {
		p = buf + buflen;
		if (prepend(&p, &buflen, "//deleted", 10) != 0)
			goto Elong;
		buflen++;
	}
	retval = __dentry_path(dentry, buf, buflen);
	if (!IS_ERR(retval) && p)
		*p = '/';	/* restore '/' overriden with '\0' */
	return retval;
Elong:
	return ERR_PTR(-ENAMETOOLONG);
}
