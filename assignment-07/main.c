// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

#define FOLD_NAME "fortytwo"

static struct dentry *folder;


/* jiffies */

char *jbuf = NULL;
unsigned int jsize = 0;

static int jiffies_open(struct inode *inode, struct file *file)
{
	unsigned long tmp = jiffies;

	while (tmp > 9) {
		jsize++;
		tmp/=10;
	}

	jbuf = kmalloc(jsize, GFP_KERNEL);
	snprintf(jbuf, 10, "%ld", jiffies);

	return 0;
}

static ssize_t jiffies_read(struct file *file, char __user *user_buffer,
			 size_t size, loff_t *offset)
{
	int ret = 0;

	size = size > jsize ? jsize : size;
	if (*offset >= jsize)
		return ret;
	ret = copy_to_user(user_buffer, jbuf + *offset, size);
	*offset += size-ret;
	return size-ret;
}

static int jiffies_close(struct inode *inodep, struct file *filep)
{
	kfree(jbuf);
	jbuf = NULL;
	jsize = 0;
	return 0;
}

const struct file_operations jiffies_fops = {
	.open = jiffies_open,
	.read = jiffies_read,
	.release = jiffies_close
};

/* id file */

static const char *login = "lumenthi";
static const int login_len = 8;

static ssize_t id_read(struct file *file, char __user *user_buffer,
			 size_t size, loff_t *offset)
{
	int ret = 0;

	size = size > login_len ? login_len : size;
	if (*offset >= login_len)
		return ret;
	ret = copy_to_user(user_buffer, login + *offset, size);
	*offset += size-ret;
	return size-ret;
}

static ssize_t id_write(struct file *file, const char __user *user_buffer,
			  size_t size, loff_t *offset)
{
	if (size == login_len && !strncmp(login, user_buffer, size))
		return size;
	return -EINVAL;
}

const struct file_operations id_fops = {
	.read = id_read,
	.write = id_write,
};

/* Operations */

static int fortytwo_init(void)
{
	int ret = 0;

	folder = debugfs_create_dir(FOLD_NAME, NULL);
	if (!folder) {
		pr_err("[*] Can't create %s fortytwo folder\n", FOLD_NAME);
		ret = -1;
	}
	debugfs_create_file("id", 0666, folder, NULL, &id_fops);
	debugfs_create_file("jiffies", 0444, folder, NULL, &jiffies_fops);
	return ret;
}

static void fortytwo_exit(void)
{
	if (jbuf)
		kfree(jbuf);
	debugfs_remove_recursive(folder);
	debugfs_remove(folder);
}

module_init(fortytwo_init);
module_exit(fortytwo_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Misc module");
MODULE_AUTHOR("lumenthi");
