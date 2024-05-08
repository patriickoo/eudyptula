// SPDX-License-Identifier: GPL-2.0+

#include <linux/cleanup.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/rwsem.h>
#include <linux/sysfs.h>
#include <linux/vmalloc.h>

#define USERID	"pat\n"

static DECLARE_RWSEM(foo_lock);

static char foo_data[PAGE_SIZE];

static struct kobject *hello_kobj;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	return sysfs_emit(buf, USERID);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	if (count != sizeof(USERID) - 1)
		return -EINVAL;

	pr_debug("hello: /sys/kernel/debug/eudyptula/id: received %s", buf);

	if (strncmp(buf, USERID, count))
		return -EINVAL;

	return count;
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	guard(rwsem_read)(&foo_lock);

	return sysfs_emit(buf, foo_data);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	guard(rwsem_write)(&foo_lock);
	strcpy(foo_data, buf);

	return count;
}

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	return sysfs_emit(buf, "%ld\n", jiffies);
}

static struct kobj_attribute id_attr = __ATTR(id, 0664, id_show, id_store);
static struct kobj_attribute jiffies_attr = __ATTR_RO(jiffies);
static struct kobj_attribute foo_attr = __ATTR_RW(foo);

static struct attribute *attrs[] = {
	&id_attr.attr,
	&jiffies_attr.attr,
	&foo_attr.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init hello_init(void)
{
	int ret;

	hello_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!hello_kobj)
		return -ENOMEM;

	ret = sysfs_create_group(hello_kobj, &attr_group);
	if (ret)
		kobject_put(hello_kobj);

	pr_debug("hello: Hello, world!\n");

	return ret;
}

static void __exit hello_exit(void)
{
	kobject_put(hello_kobj);
	pr_debug("hello: Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Patrick Barsanti <patrick.barsanti@amarulasolutions.com");
MODULE_LICENSE("GPL v2");

