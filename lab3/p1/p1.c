#include <linux/init.h>
#include <linux/module.h>

static int __init myinit(void)
{
    return 0;
}

static void __exit myexit(void)
{}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gets wifi adapter's mac-address");
MODULE_AUTHOR("nia40m");
