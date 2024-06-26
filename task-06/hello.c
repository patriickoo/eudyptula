// SPDX-License-Identifier: GPL-2.0+

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#define USERID	"pat\n"

static ssize_t hello_misc_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos,
				       USERID, sizeof(USERID));
}

static ssize_t hello_misc_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	char str[sizeof(USERID)];
	int ret;

	if (count != sizeof(USERID) - 1)
		return -EINVAL;

	ret = simple_write_to_buffer(str, count, ppos, buf, count);

	pr_debug("hello: /dev/eudyptula: device received %s", str);

	if (strncmp(str, USERID, count))
		return -EINVAL;

	return count;
}

static const struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = hello_misc_read,
	.write = hello_misc_write,
};

static struct miscdevice hello_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &hello_fops,
};

static int __init hello_init(void)
{
	int ret;

	ret = misc_register(&hello_misc);
	if (ret)
		return ret;

	pr_debug("hello: Hello, world!\n");

	return ret;
}

static void __exit hello_exit(void)
{
	misc_deregister(&hello_misc);
	pr_debug("hello: Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");

