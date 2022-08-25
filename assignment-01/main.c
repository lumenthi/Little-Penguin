#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_DEBUG "Hello world !\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_DEBUG "Cleaning up module.\n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello World module");
MODULE_AUTHOR("lumenthi");
