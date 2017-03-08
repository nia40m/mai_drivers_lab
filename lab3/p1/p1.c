#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <asm/io.h>

#define BASE_ADDR 0xf0100000
#define ADDR_LENG 0x10000
#define MAC_ADDR  0x8000

static int __init myinit(void)
{
    void *mem;

    mem = ioremap(BASE_ADDR, ADDR_LENG);

    pr_info("%x\n", ioread32(mem + MAC_ADDR));
    pr_info("%x\n", ioread16(mem + MAC_ADDR + 4));

    iounmap(mem);

    return 0;
}

static void __exit myexit(void) {}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gets wifi adapter's mac address");
MODULE_AUTHOR("nia40m");
