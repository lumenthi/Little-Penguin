#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_INFO "Hello keyboard !\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye keyboard !\n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Say hello USB keyboards");
MODULE_AUTHOR("lumenthi");
