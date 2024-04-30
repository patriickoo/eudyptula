// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>

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

