#include <linux/kernel.h>
#include <linux/module.h>

void bar(void)
{
	printk("bar is unused code\n");
}

static int __init test_init(void)
{
	bool use_bar = false;

	if (use_bar)
		bar();
	/*
	 * XXX: demo different ways to retire bar().
	 */

	printk("hello!\n");
	return 0;
}

static void __exit test_exit(void)
{
	printk("goodbye!\n");
}

module_init(test_init)
module_exit(test_exit)
MODULE_LICENSE("GPL");
