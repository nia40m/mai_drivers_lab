#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

/* USB THINGS */
static struct usb_device_id my_usb_ids[] = {
    { USB_DEVICE(0x2001, 0x5678) },
    { 0 }
};
MODULE_DEVICE_TABLE(usb, my_usb_ids);

static int  myprobe(struct usb_interface *intr, const struct usb_device_id *id);
static void mydisc(struct usb_interface *intr);

static struct usb_driver my_usb_driver = {
    .name       = "n4_mouse",
    .id_table   = my_usb_ids,
    .probe      = myprobe,
    .disconnect = mydisc,
};



static int __init myinit(void)
{
    int status;

    status = usb_register(&my_usb_driver);
    if (status) {
        pr_info("Usb can't register my driver");
        return -1;
    }

    return 0;
}

static void __exit myexit(void)
{
    usb_deregister(&my_usb_driver);
}

static int  myprobe(struct usb_interface *intr, const struct usb_device_id *id)
{
    return 0;
}

static void mydisc(struct usb_interface *intr)
{

}



module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Count number of mouse left-clicks");
MODULE_AUTHOR("nia40m");
