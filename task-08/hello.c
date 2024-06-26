// SPDX-License-Identifier: GPL-2.0+

#include <linux/cleanup.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/rwsem.h>

#define USERID	"pat\n"

static DECLARE_RWSEM(foo_lock);

static struct dentry *ddir;
static char foo_data[PAGE_SIZE];
static int foo_data_len;

static ssize_t id_read(struct file *filp, char __user *buf,
		       size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos,
				       USERID, sizeof(USERID));
}

static ssize_t id_write(struct file *filp, const char __user *buf,
			size_t count, loff_t *ppos)
{
	char str[sizeof(USERID)];
	int ret;

	if (count != sizeof(USERID) - 1)
		return -EINVAL;

	ret = simple_write_to_buffer(str, count, ppos, buf, count);

	pr_debug("hello: /sys/kernel/debug/eudyptula/id: received %s\n", str);

	if (strncmp(str, USERID, count))
		return -EINVAL;

	return count;
}

static ssize_t foo_read(struct file *filp, char __user *buf,
				size_t count, loff_t *ppos)
{
	guard(rwsem_read)(&foo_lock);

	return simple_read_from_buffer(buf, count, ppos,
				       foo_data, foo_data_len);
}

static ssize_t foo_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	guard(rwsem_write)(&foo_lock);
	foo_data_len = count;

	return simple_write_to_buffer(foo_data, PAGE_SIZE, ppos, buf, count);
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static int __init hello_init(void)
{
	struct dentry *debugfile;

	ddir = debugfs_create_dir("eudyptula", NULL);

	if (IS_ERR(ddir))
		return PTR_ERR(ddir);

	debugfile = debugfs_create_file(
			"id", 0666, ddir, NULL, &id_fops);

	if (IS_ERR(debugfile)) {
		debugfs_remove_recursive(ddir);
		return PTR_ERR(debugfile);
	}

	debugfs_create_ulong("jiffies", 0444, ddir, &jiffies);

	debugfile = debugfs_create_file(
			"foo", 0644, ddir, &foo_data, &foo_fops);

	if (IS_ERR(debugfile)) {
		debugfs_remove_recursive(ddir);
		return PTR_ERR(debugfile);
	}

	pr_debug("hello: Hello, world!\n");

	return 0;
}

static void __exit hello_exit(void)
{
	debugfs_remove_recursive(ddir);
	pr_debug("hello: Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");

