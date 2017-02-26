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
/* end of defenition*/

/* fops for all users */
static struct file_operations mai_pipe_fops = {
    .owner = THIS_MODULE,
    .open  = mai_pipe_open,
};
/* fops for superuser
static struct file_operations mai_pipe_fops_superuser = {

};
*/


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

    return -1;
}



module_init(mai_pipe_init);
module_exit(mai_pipe_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("nia40m's implementation of MAI_PIPE");
MODULE_AUTHOR("nia40m");
