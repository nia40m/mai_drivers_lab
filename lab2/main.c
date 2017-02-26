#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define MAI_DEVICE_NUMBERS 1

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("nia40m's implementation of MAI_PIPE");
MODULE_AUTHOR("nia40m");

static int buff_size = 128;
module_param(buff_size, int, 0000);

dev_t m_nums = MKDEV(666, 666);

static int __init mai_pipe_init(void)
{
    int status;

    status = register_chrdev_region(m_nums, MAI_DEVICE_NUMBERS, "mai_pipe");
    if (status < 0)
        return -1;

    return 0;
}

static void __exit mai_pipe_exit(void)
{
    unregister_chrdev_region(m_nums, MAI_DEVICE_NUMBERS);
}

module_init(mai_pipe_init);
module_exit(mai_pipe_exit);
