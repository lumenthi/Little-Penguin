#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

static char *read_message = "lumenthi";
static int message_length = 8;

static ssize_t misc_read(struct file *file, char __user *user_buffer,
						size_t size, loff_t *offset)
{
	int ret = -1;
	// First way to do it
	// ret = simple_read_from_buffer(user_buffer, size, offset, read_message, message_length);
	// printk(KERN_INFO "[*] simple_read: %d\n", ret);
	// return ret;

	// Second way
	printk(KERN_INFO "[*] Offset value: %lld\n", *offset);
	if (*offset >= message_length) // Check is there more bytes to send to user
		return 0; // read will not stop until the func rets "0" or error
	ret = copy_to_user(user_buffer, read_message, message_length);
	// ret contains the amount of char that wasnt successfully written
	printk(KERN_INFO "[*] Read %d characters\n", message_length - ret);
	*offset += message_length - ret; // incrementing user_buffer offp now that we have read
	return message_length;
}

static ssize_t misc_write(struct file *file, const char __user *user_buffer,
						size_t size, loff_t *offset)
{
	int ret = -EINVAL; // Negative ret for error with value INVALID ARG
	printk(KERN_INFO "[*] User Buffer Value: %s\n", user_buffer);
	// printk(KERN_INFO "[*] Size: %ld, Msg Length: %d\n", size, message_length);
	if (size == message_length && !strncmp(read_message, user_buffer, size)) {
		printk(KERN_INFO "[*] Valid string\n");
		return size;
	}
	return ret;
}

const struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.read = misc_read,
	.write = misc_write,
};

static struct miscdevice misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &misc_fops, // file_operations
};

static int misc_init(void)
{
	int ret = -1;
	printk(KERN_INFO "[*] Misc Module Constructor\n");
	ret = misc_register(&misc_device); // Registering my device
	return ret;
}

static void misc_exit(void)
{
	printk(KERN_INFO "[*] Misc Module Destructor\n");
	misc_deregister(&misc_device); // Removing my device
}

module_init(misc_init); // Function called when init module
module_exit(misc_exit); // Function called when exit module

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Misc module");
MODULE_AUTHOR("lumenthi");
