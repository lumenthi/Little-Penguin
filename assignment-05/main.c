#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define DEV_NAME "fortytwo"

static const char *login = "lumenthi";
static const int login_len = 8;

static ssize_t misc_read(struct file *file, char __user *user_buffer,
			 size_t size, loff_t *offset)
{
	int ret = 0;

	size = size > login_len ? login_len: size;
	if (*offset >= login_len)
		return ret;
	ret = copy_to_user(user_buffer, login+*offset, size);
	*offset += size-ret;
	return size-ret;
}

static ssize_t misc_write(struct file *file, const char __user *user_buffer,
			  size_t size, loff_t *offset)
{
	printk(KERN_INFO "[*] User Buffer Value: %s\n", user_buffer);
	// printk(KERN_INFO "[*] Size: %ld, Msg Length: %d\n", size, login_len);
	if (size == login_len && !strncmp(login, user_buffer, size)) {
		printk(KERN_INFO "[*] Valid string\n");
		return size;
	}
	return -EINVAL;
}

static int misc_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "[*] Opening %s\n", DEV_NAME);
	return 0;
}

static int misc_close(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "[*] Closing %s\n", DEV_NAME);
	return 0;
}

const struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.open = misc_open,
	.read = misc_read,
	.write = misc_write,
	.release = misc_close,
};

static struct miscdevice misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEV_NAME,
	.fops = &misc_fops,
};

static int misc_init(void)
{
	int ret = -1;
	printk(KERN_INFO "[*] Misc module constructor\n");
	ret = misc_register(&misc_device);
	if (ret < 0)
		printk(KERN_ERR "[*] Can't register misc_device\n");
	return ret;
}

static void misc_exit(void)
{
	printk(KERN_INFO "[*] Misc module destructor\n");
	misc_deregister(&misc_device);
}

module_init(misc_init);
module_exit(misc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Misc module");
MODULE_AUTHOR("lumenthi");
