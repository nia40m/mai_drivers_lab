#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pci-aspm.h>
#include <asm/io.h>

#include <linux/delay.h>

/* PCI THINGS */
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
#define MAC_ADDR_OFFSET  0x8000
#define MAC_LENGHT  6
#define BUFF_LENGHT 256

static char mac_address[MAC_LENGHT];



static int myprobe(struct pci_dev *dev, const struct pci_device_id *id)
{
    u32 data;
    int i;
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

    for (i = 0x1f; i >= 0x1d; i--) {
        iowrite32(i, mem + 0x6000);
        udelay(20);
        iowrite32(ioread32(mem + 0x6008) | 1, mem + 0x6008);
        udelay(10000);
        pr_info("status: 0x%x\n", ioread32(mem + 0x600c));
        pr_info("data: 0x%x\n", ioread32(mem + 0x6004));
    }
/*
    for (i = 0; i < MAC_LENGHT; i++) {
        mac_address[i] = ioread8(mem + MAC_ADDR_OFFSET + i);
        pr_info("%x\t", mac_address[i]);
    }
*/
    iounmap(mem);

    return 0;
}

static void myremove(struct pci_dev *dev)
{
    pci_disable_device(dev);
    pci_release_region(dev, 0);
}

static int __init myinit(void)
{
    int status;

    status = pci_register_driver(&my_pci_driver);
    if (status) {
        pr_info("n4: Can't register driver in pci system\n");
        return -1;
    }

    return 0;
}

static void __exit myexit(void)
{
    pci_unregister_driver(&my_pci_driver);
}



module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gets wifi adapter's mac-address");
MODULE_AUTHOR("nia40m");
