// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/hid.h>

static struct usb_device_id hello_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }
};
MODULE_DEVICE_TABLE(usb, hello_table);

static int __init hello_init(void)
{
	pr_debug("Hello, world!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_debug("Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");

