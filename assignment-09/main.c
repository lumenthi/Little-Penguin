// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/mount.h>
#include <linux/uaccess.h>
#include "main.h"

#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME		"mymounts"

static struct proc_dir_entry *proc_file;

static ssize_t procfile_read(struct file *file, char __user *user_buffer,
		      size_t size, loff_t *offset)
{
	return 0;
}

static int procfile_open(struct inode *inode, struct file *file)
{
	struct nsproxy *nsp;
	struct mnt_namespace *mnt_ns;
	struct mount *mnt_current;
	struct fs_struct *fs_root;
	struct path path_root;
	char *name;

	pr_info("[*] Open procfile\n");

	fs_root = current->fs;

	nsp = current->nsproxy;
	mnt_ns = nsp->mnt_ns;

	get_mnt_ns(mnt_ns);

	get_fs_root(fs_root, &path_root);

	pr_info("[*] Mounts: %d\n", mnt_ns->mounts);
	pr_info("[*] Path: %s\n", path_root.dentry->d_name.name);

	list_for_each_entry(mnt_current, &mnt_ns->list, mnt_list) {
		name = mnt_current->mnt_devname;
		pr_info("[*] Name: %s\n", mnt_current->mnt_devname);
		if (!name)
			return 0;
	}

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
