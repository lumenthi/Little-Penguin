#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME		"mymounts"

static struct proc_dir_entry *proc_file;

static char procfs_buffer[PROCFS_MAX_SIZE];

static unsigned long procfs_buffer_size = 0;

ssize_t procfile_read(struct file *file, char __user *user_buffer,
						size_t size, loff_t *offset)
{
	int ret = 1;
	if (*offset > 0) // read finished, return 0
		return 0;
	ret = copy_to_user(user_buffer, procfs_buffer, procfs_buffer_size);
	*offset += procfs_buffer_size - ret;
	return procfs_buffer_size - ret;
}

static const struct file_operations proc_file_fops = {
	.owner = THIS_MODULE,
	.read = procfile_read,
};

static int proc_init(void)
{
	int ret = 0;
	printk(KERN_INFO "[*] Proc Module Constructor\n");
	//char *tmp = *(char**)&procfs_buffer;
	char *tmp = (char*)&procfs_buffer;
	tmp = "a";
	//procfs_buffer[0] = 'a';
	//procfs_buffer_size += 1;
	printk(KERN_INFO "%p\n", procfs_buffer);
	printk(KERN_INFO "%p\n", tmp);
	// METHOD FOR KERNEL > 3.10 Version
	proc_file = proc_create(PROCFS_NAME, 0664, NULL, &proc_file_fops);
	if (proc_file == NULL)
		return -ENOMEM;	
	// METHOD FOR KERNEL < 3.10 Version
	/* proc_file = create_proc_entry(PROCFS_NAME, 0644, NULL);*/
	/*if (proc_file == NULL) {*/
		/*remove_proc_entry(PROCFS_NAME, &proc_root); // &proc_root = parent dir*/
		/*return -ENOMEM // memory error*/
	/*}*/
	/*proc_file->read_proc = procfile_read;*/
	/*// proc_file->write_proc = procfile_write;*/
	/*proc_file->owner = THIS_MODULE;*/
	/*proc_file->mode = S_IFREG | S_IRUGO // regular file | can be read by everybody but cannot be changed*/
	/*proc_file->uid = 0; // root*/
	/*proc_file->gid = 0; // root*/
	/*proc_file->size = 80;*/
	return ret;
}

static void proc_exit(void)
{
	printk(KERN_INFO "[*] Proc Module Destructor\n");
	proc_remove(proc_file);
}

module_init(proc_init); // Function called when init module
module_exit(proc_exit); // Function called when exit module

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mymounts module");
MODULE_AUTHOR("lumenthi");
