// SPDX-License-Identifier: GPL-2.0+

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Reverse module");

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size,
			 loff_t *offs);

static ssize_t myfd_write(struct file *fp, const char __user *user,
			  size_t size, loff_t *offs);

static const struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE];

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&myfd_device);
	if (retval < 0)
		pr_err("[*] Can't register reverse misc_device\n");
	return retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	size_t i;
	size_t len;
	char *tmp;
	ssize_t ret;

	/*
	 * Malloc like a boss
	 */
	tmp = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
	if (tmp == NULL)
		return -ENOMEM;
	i = 0;
	len = strlen(str);
	while (i < len) {
		tmp[i] = str[len-i-1];
		i++;
	}
	tmp[i] = 0x0;

	ret = simple_read_from_buffer(user, size, offs, tmp, i);
	kfree(tmp);
	return ret;
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
		   loff_t *offs)
{
	ssize_t res;

	res = simple_write_to_buffer(str, size, offs, user, size);
	str[size] = 0x0;
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
