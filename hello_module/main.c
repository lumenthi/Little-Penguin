#include <linux/module.h> // Needed by modules
#include <linux/kernel.h> // KERN_INFO

int init_module(void) {
	printk(KERN_INFO "HELLO\n");
	return 0;
}

void cleanup_module(void) {
	printk(KERN_INFO "GOODBYE\n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello");
MODULE_AUTHOR("lumenthi");
