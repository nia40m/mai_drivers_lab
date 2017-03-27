#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pci-aspm.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

/* PCI THINGS */
#define EEPROM_ADDR_REG 0x6000
#define EEPROM_READ_REG 0x6008
#define EEPROM_STAT_REG 0x600c
#define EEPROM_DATA_REG 0x6004

static struct pci_device_id my_pci_ids[] = {
    { PCI_VDEVICE(ATHEROS, 0x001c) },
    { 0 }
};
MODULE_DEVICE_TABLE(pci, my_pci_ids);

static int  myprobe(struct pci_dev *dev, const struct pci_device_id *id);
static void myremove(struct pci_dev *dev);

static struct pci_driver my_pci_driver = {
    .name     = "n4_wifi",
    .id_table = my_pci_ids,
    .probe    = myprobe,
    .remove   = myremove,
};

/* MAC-address THINGS */
#define EEPROM_MAC_ADDR_FIRST   0x1f
#define EEPROM_MAC_ADDR_LAST    0x1d
#define MAC_LENGHT  6
#define BUFF_LENGHT 256

static char mac_address[MAC_LENGHT];
static char hex_symb[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

static ssize_t myread(struct file *file, char *buff, size_t size, loff_t *off);

static struct file_operations my_fops = {
    .read = myread,
};



static int myprobe(struct pci_dev *dev, const struct pci_device_id *id)
{
    u32 data;
    int i, j = 0;
    int status;
    void *mem;

    status = pci_enable_device(dev);
    if (status) {
        pr_info("n4: PCI can't enable device\n");
        return -1;
    }

    status = pci_request_region(dev, 0, "n4_wifi");
    if (status) {
        pr_info("n4: PCI can't reserve region for my driver\n");
        return -1;
    }

    mem = ioremap(pci_resource_start(dev, 0), pci_resource_len(dev, 0));

    for (i = EEPROM_MAC_ADDR_FIRST; i >= EEPROM_MAC_ADDR_LAST; i--) {
        iowrite32(i, mem + EEPROM_ADDR_REG);
        udelay(20);

        iowrite32(ioread32(mem + EEPROM_READ_REG) | 1, mem + EEPROM_READ_REG);
        udelay(10000);

        data = ioread32(mem + EEPROM_STAT_REG);
        if (data & 2) {
            data = ioread32(mem + EEPROM_DATA_REG) & 0xffff;
            mac_address[j++] = (data & 0xff00) >> 8;
            mac_address[j++] = data & 0x00ff;
        }
    }

    iounmap(mem);

    return 0;
}

static void myremove(struct pci_dev *dev)
{
    pci_disable_device(dev);
    pci_release_region(dev, 0);
}

static dev_t  my_dev_t;
static struct cdev   my_cdev;
static struct class  *my_class;
static struct device *my_device;

static int __init myinit(void)
{
    int status;

    status = pci_register_driver(&my_pci_driver);
    if (status) {
        pr_info("n4: Can't register driver in pci system\n");
        return -1;
    }

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

    return 0;
}

static void __exit myexit(void)
{
    pci_unregister_driver(&my_pci_driver);
    device_destroy(my_class, my_dev_t);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev_t, 1);
}

static int mac_to_str(char *buff)
{
    int i, j;

    for (i = j = 0; i < MAC_LENGHT; i++) {
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

    temp = devm_kmalloc(my_device, BUFF_LENGHT, GFP_KERNEL);
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
