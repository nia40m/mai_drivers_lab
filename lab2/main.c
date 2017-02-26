#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cred.h>

#define MAI_SUPERUSER_UID 0
#define MAI_BUFF_SIZE_DEFAULT 128
#define MAI_MAJOR_NUMBER 666
#define MAI_DEVICE_NAME "mai_pipe"

static int buff_size = MAI_BUFF_SIZE_DEFAULT;
module_param(buff_size, int, 0000);

/* defenition of all file_operation functions */
static int mai_pipe_open(struct inode *inode, struct file *file);

static ssize_t mai_pipe_read_superuser
(struct file *file, char *buff, size_t size, loff_t *off);

static ssize_t mai_pipe_write_superuser
(struct file *file, const char *buff, size_t size, loff_t *off);
/* end of defenition*/

/* fops for all users */
static struct file_operations mai_pipe_fops = {
    .owner = THIS_MODULE,
    .open  = mai_pipe_open,
};

/* fops for superuser */
static struct file_operations mai_pipe_fops_superuser = {
    .owner = THIS_MODULE,
    .open  = mai_pipe_open,
    .read  = mai_pipe_read_superuser,
    .write = mai_pipe_write_superuser,
};



static int __init mai_pipe_init(void)
{
    int status;

    status = register_chrdev(MAI_MAJOR_NUMBER, MAI_DEVICE_NAME, &mai_pipe_fops);
    if (status < 0)
        return -1;

    return 0;
}

static void __exit mai_pipe_exit(void)
{
    unregister_chrdev(MAI_MAJOR_NUMBER, MAI_DEVICE_NAME);
}

static int mai_pipe_open(struct inode *inode, struct file *file)
{
    const struct cred *cred;
    int uid;

    /* getting credential to obtain process real UID */
    cred = get_cred(file->f_cred);
    uid  = cred->uid.val;
    put_cred(cred);

    /* replacement of fops for superuser */
    if (uid == MAI_SUPERUSER_UID) {
        file->f_op = &mai_pipe_fops_superuser;
        return 0;
    }

    return -1;
}

static ssize_t mai_pipe_read_superuser
(struct file *file, char *buff, size_t size, loff_t *off)
{
    pr_info("Superuser is trying to read info! "
        "Ha-ha, RETARD! YOU CAN'T DO SUCH THING!\n");

    return 0;
}

static ssize_t mai_pipe_write_superuser
(struct file *file, const char *buff, size_t size, loff_t *off)
{
    pr_info("Superuser is trying to write info! "
        "USE LINUX KERNEL PIPE, SCALLYWAG!\n");

    return (ssize_t) size;
}



module_init(mai_pipe_init);
module_exit(mai_pipe_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("nia40m's implementation of MAI_PIPE");
MODULE_AUTHOR("nia40m");
