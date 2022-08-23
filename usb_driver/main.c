#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// usb_device_id
static struct usb_device_id hello_table[] = {
	{ USB_INTERFACE_INFO(3, 1, 1) }, // 3 bInterfaceClass -> HUMAN INTERFACE DEVICE
									 // 1 bInterfaceSubclass -> BOOT INTERFACE SUBCLASS
									 // 1 bInterfaceProtocol -> KEYBOARD
									 // Used lssub -v to get Interface Descriptor of my keyboard
									 // So my driver will basically target only keyboards
	{ }
};

MODULE_DEVICE_TABLE (usb, hello_table); // FIRST PARAMETER IS TYPE OF DEVICE
										// SECOND IS THE TABLE
										// Add my driver to modules.usbmap

// probe function
// called on device insertion if and only if no other driver has beat us
static int hello_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	printk(KERN_INFO "[*] Keyboard (%x), plugged in\n", id->idProduct);
	return 0;
}

// disconnect function
// called on device disconnect
static void hello_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "[*] Keyboard unplugged\n");
}

// usb_driver
static struct usb_driver hello_driver =
{
	.name = "Hello Module",
	.id_table = hello_table,
	.probe = hello_probe,
	.disconnect = hello_disconnect,
};

static int __init hello_init(void)
{
	int ret = -1;
	printk(KERN_INFO "[*] Hello Module Constructor\n");
	ret = usb_register(&hello_driver); // Registering my driver
	return ret;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "[*] Hello Module Destructor\n");
	usb_deregister(&hello_driver); // Removing my driver
}

module_init(hello_init); // Function called when init module
module_exit(hello_exit); // Function called when exit module

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello module");
MODULE_AUTHOR("lumenthi");
