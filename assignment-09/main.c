// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/mount.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "main.h"

#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME		"mymounts"

static struct proc_dir_entry *proc_file;

/* Buffer for path operations */
static char pbuf[PATH_MAX];
/* Return buffer */
static unsigned int rsize;
static char *rbuf;

static ssize_t procfile_read(struct file *file, char __user *user_buffer,
		      size_t size, loff_t *offset)
{
	int ret = 0;

	size = size > rsize ? rsize : size;
	if (*offset >= rsize)
		return ret;
	ret = copy_to_user(user_buffer, rbuf + *offset, size);
	*offset += size-ret;
	return size-ret;
}

static unsigned int total_size(struct mnt_namespace *mnt_ns)
{
	struct mount *mnt_current;
	unsigned int ret = 0;

	list_for_each_entry(mnt_current, &mnt_ns->list, mnt_list) {
		memset(pbuf, 0, sizeof(pbuf));
		ret += strlen(mnt_current->mnt_devname);
		ret += strlen(dentry_path(mnt_current->mnt_mountpoint, pbuf, sizeof(pbuf)));
		ret += 2;
	}
	return ret;
}

static unsigned int get_name(const char *name, unsigned int offset)
{
	char *tmp = rbuf+offset;
	unsigned int len = strlen(name);

	strncpy(tmp, name, len);
	tmp[len] = ' ';
	return len+1;
}

static unsigned int get_path(struct dentry *dentry, unsigned int offset)
{
	char *path;
	char *tmp = rbuf+offset;
	unsigned int len;

	memset(pbuf, 0, sizeof(pbuf));
	path = dentry_path(dentry, pbuf, sizeof(pbuf));

	len = strlen(path);
	strncpy(tmp, path, len);
	tmp[len] = '\n';
	return len+1;
}

static int procfile_open(struct inode *inode, struct file *file)
{
	struct nsproxy *nsp;
	struct mnt_namespace *mnt_ns;
	struct mount *mnt_current;
	unsigned int offset = 0;

	nsp = current->nsproxy;
	mnt_ns = nsp->mnt_ns;
	get_mnt_ns(mnt_ns);

	rsize = total_size(mnt_ns);

	rbuf = kmalloc(rsize, GFP_KERNEL);

	list_for_each_entry(mnt_current, &mnt_ns->list, mnt_list) {
		offset += get_name(mnt_current->mnt_devname, offset);
		offset += get_path(mnt_current->mnt_mountpoint, offset);
	}
	return 0;
}

static int procfile_close(struct inode *inodep, struct file *filep)
{
	kfree(rbuf);

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
	proc_file = proc_create(PROCFS_NAME, 0664, NULL, &proc_file_fops);
	if (proc_file == NULL)
		return -ENOMEM;
	return 0;
}

static void proc_exit(void)
{
	proc_remove(proc_file);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mymounts module");
MODULE_AUTHOR("lumenthi");
