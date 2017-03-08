#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define BASE_ADDR 0xf0100000
#define ADDR_LENG 0x10000
#define MAC_ADDR  0x8000
#define MAC_LENG  12
#define BUFF_LENG 256

static char mac_address[MAC_LENG];



static ssize_t myread(strcut file *file, char *buff, size_t size, loff_t *off);

static struct file_operations {
    .read = myread,
} my_fops;

static dev_t  my_dev_t;
static cdev   my_cdev;
static struct class  *my_class;
static struct device *my_device;

static int __init myinit(void)
{
    int status;
    void *mem;

    status = alloc_chrdev_region(&my_dev_t, 0, 1, "n4_wifi");
    if (status < 0)
        return -1;

    cdev_init(&my_cdev, &my_fops);
    status = cdev_add(&my_cdev, my_dev_t, 1);
    if (status < 0) {
        unregister_chrdev_region(my_dev_t, 1);
        return -1;
    }

    my_class  = class_create(THIS_MODULE, "n4 class");
    my_device = device_create(my_class, NULL, my_dev_t, NULL, "n4_wifi");

    /* getting mac-address */
    mem = ioremap(BASE_ADDR, ADDR_LENG);
    ioread8_rep(mem + MAC_ADDR, mac_address, MAC_LENG);
    iounmap(mem);

    return 0;
}

static void __exit myexit(void)
{
    device_destroy(my_class, my_dev_t);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev_t, 1);
}

static int mac_to_str(char *buff)
{
    int i, j;

    for (i = j = 0; i < MAC_LENG; i++) {
        buff[j++] = (mac_address[i] >> 4)   + '0';
        buff[j++] = (mac_address[i] & 0x0f) + '0';
        buff[j++] = ':';
    }
    buff[--j] = 0;

    return j;
}

static ssize_t myread(strcut file *file, char *buff, size_t size, loff_t *off)
{
    int num;
    char *temp;

    temp = devm_kmalloc(&my_device, BUFF_LENG, GFP_KERNEL);
    if (temp == NULL)
        return -1;

    num = mac_to_str(temp);

    if (size < num) {
        devm_kfree(temp);
        return 0;
    }

    copy_to_user(buff, temp, num);
    devm_kfree(temp);

    return (ssize_t) num;
}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gets wifi adapter's mac-address");
MODULE_AUTHOR("nia40m");
