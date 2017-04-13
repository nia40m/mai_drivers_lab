#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define BUFF_SIZE 256
#define KBD_IRQ_NUM 1

static int num_key_pressed;

static ssize_t myread(struct file *file, char *buff, size_t size, loff_t *off);

static struct file_operations my_fops = {
    .read = myread,
};



static irqreturn_t myirq_handler(int irq, void *dev_id)
{
    num_key_pressed++;
    return IRQ_NONE;
}

static dev_t  my_dev_t;
static struct cdev   my_cdev;
static struct class  *my_class;
static struct device *my_device;

static int __init myinit(void)
{
    int status;

    status = request_irq(KBD_IRQ_NUM, myirq_handler,
        IRQF_SHARED, "n40_int", &num_key_pressed);
    if (status)
        return status;

    status = alloc_chrdev_region(&my_dev_t, 0, 1, "n4_int");
    if (status < 0)
        return -1;

    cdev_init(&my_cdev, &my_fops);
    status = cdev_add(&my_cdev, my_dev_t, 1);
    if (status < 0) {
        unregister_chrdev_region(my_dev_t, 1);
        return -1;
    }

    my_class  = class_create(THIS_MODULE, "n4 class");
    my_device = device_create(my_class, NULL, my_dev_t, NULL, "n4_int");

    return 0;
}

static void __exit myexit(void)
{
    free_irq(KBD_IRQ_NUM, &num_key_pressed);
    device_destroy(my_class, my_dev_t);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev_t, 1);
}

static ssize_t myread(struct file *file, char *buff, size_t size, loff_t *off)
{
    int status;
    int len;
    char str[BUFF_SIZE];

    if (*off != 0)
        return 0;

    len = sprintf(str, "Number of interrupts: %d\n", num_key_pressed);

    pr_info("%s", str);

    status = copy_to_user(buff, str, len);
    if (status)
        return -1;

    *off += len;
    return (ssize_t) len;
}



module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sets irq handler to keyboard");
MODULE_AUTHOR("nia40m");
