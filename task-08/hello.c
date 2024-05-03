// SPDX-License-Identifier: GPL-2.0+

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/module.h>

#define USERID	"pat\n"

static struct dentry *ddir;

static ssize_t id_read(struct file *filp, char __user *buf,
				size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos,
			USERID, strlen(USERID));
}

static ssize_t id_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	if (count != strlen(USERID))
		return -EINVAL;

	char str[strlen(USERID)];
	int ret;

	ret = simple_write_to_buffer(str, count, ppos, buf, count);

	pr_debug("Device received: %s\n", str);

	if (strncmp(str, USERID, count))
		return -EINVAL;

	return count;
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static int __init hello_init(void)
{
	struct dentry *debugfile;

	ddir = debugfs_create_dir("eudyptula", NULL);

	if (IS_ERR(ddir))
		return PTR_ERR(ddir);

	debugfile = debugfs_create_file(
			"id", 0666, ddir, NULL, &id_fops);

	if (IS_ERR(debugfile))
		return PTR_ERR(debugfile);

	debugfs_create_ulong("jiffies", 0444, ddir, &jiffies);

	pr_debug("Hello, world!\n");

	return 0;
}

static void __exit hello_exit(void)
{
	debugfs_remove_recursive(ddir);
	pr_debug("Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");

