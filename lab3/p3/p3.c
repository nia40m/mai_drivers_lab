#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#define KBD_IRQ_NUM 1

static int bred = 123;

static irqreturn_t myirq_handler(int irq, void *dev_id)
{
    pr_info("INTERRUPTE!\n");
    return IRQ_NONE;
}

static int __init myinit(void)
{
    int status;

    status = request_irq(KBD_IRQ_NUM, myirq_handler, IRQF_SHARED, "n40_int", &bred);
    if (status) {
        pr_info("exit here\n");
        return status;
    }
pr_info("exit there\n");
    return 0;
}

static void __exit myexit(void)
{
    free_irq(KBD_IRQ_NUM, &bred);
}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sets irq handler to keyboard");
MODULE_AUTHOR("nia40m");
