#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#define BASE_ADDR 0xf0100000
#define ADDR_LENG 0x10000
#define MAC_ADDR  0x8000
#define MAC_LENG  6
#define BUFF_LENG 256

static char mac_address[MAC_LENG];
static char hex_symb[] = {
    0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62,
    0x63, 0x64, 0x65, 0x66
};



static ssize_t myread(struct file *file, char *buff, size_t size, loff_t *off);

static struct file_operations my_fops = {
    .read = myread,
};

static dev_t  my_dev_t;
static struct cdev   my_cdev;
static struct class  *my_class;
static struct device *my_device;

static int __init myinit(void)
{
    int status, i;
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
    for (i = 0; i < MAC_LENG; i++)
        mac_address[i] = ioread8(mem + MAC_ADDR + i);
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
        buff[j++] = hex_symb[(mac_address[i] & 0xf0) >> 4];
        buff[j++] = hex_symb[mac_address[i] & 0x0f];
        buff[j++] = ':';
    }
    buff[j - 1] = '\n';

    return j;
}

static ssize_t myread(struct file *file, char *buff, size_t size, loff_t *off)
{
    int num, status;
    char *temp;

    if (*off != 0)
        return 0;

    temp = devm_kmalloc(my_device, BUFF_LENG, GFP_KERNEL);
    if (temp == NULL)
        return -1;

    num = mac_to_str(temp);

    if (size < num) {
        devm_kfree(my_device, temp);
        return 0;
    }

    status = copy_to_user(buff, temp, num);
    devm_kfree(my_device, temp);

    if (status)
        return -1;

    *off += num;
    return (ssize_t) num;
}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gets wifi adapter's mac-address");
MODULE_AUTHOR("nia40m");
