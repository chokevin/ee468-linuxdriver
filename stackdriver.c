#include <linux/init.h>
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything */
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
//#include <asm/system.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static int memory_open(struct inode *inode, struct file *filp);

static int memory_release(struct inode *inode, struct file *filp);

static ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

static ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

void memory_exit(void);

int memory_init(void);

struct file_operations memory_fops = {
    read: memory_read,
    write: memory_write,
    open: memory_open,
    release: memory_release
};

module_init(memory_init);
module_exit(memory_exit);

int memory_major = 61; 
char* memory_buffer;
int memory_buff_size = 0;

int memory_init(void) {

    int result;

    /* registering device */

    result = register_chrdev(memory_major, "stackdriver", &memory_fops);

    if (result < 0) {
        printk("<1>stackdriver: cannot obtain major number %d\n", memory_major);
        return result;
    }

    /* Allocationg Memory for the buffer */

    memory_buffer = kmalloc(10, GFP_KERNEL);

    if (!memory_buffer) {

        result = -ENOMEM;

        goto fail;

    }

    memset(memory_buffer, 0, 10);

    printk("<1>Inserting stackdriver module\n");

    return 0;

    fail:

        memory_exit();

        return result;

}


void memory_exit(void) {

    /* Freeing the major number */

    unregister_chrdev(memory_major, "stackdriver");

    if (memory_buffer) {

        kfree(memory_buffer);

    }

    printk("<1>Removing stackdriver module\n");

}


static int memory_open(struct inode *inode, struct file *filp) {

    /* Success */

    return 0;

}


static int memory_release (struct inode *inode, struct file *filp) {

    /* Success */

    return 0;

}


static ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {

    /* Transferring data to user space */
    if( (int) count <= 10) {
        copy_to_user(buf, memory_buffer+memory_buff_size- (int) count, count);
        memory_buff_size -= (int) count;
    } else {
        copy_to_user(buf, memory_buffer,10);
        memory_buff_size = 0;
    } 

    /* Changing reading position as best suits... This doesn't matter? */
    if (*f_pos == 0) {
        *f_pos += 1;
        return 1;
    } else {
        return 0;
    }
}

static ssize_t memory_write( struct file *filp, const char *buf, size_t count, loff_t *f_pos){
    const char *tmp;
    tmp = buf;
    if ((int)count <= 10) { 
        copy_from_user(memory_buffer+memory_buff_size, tmp, count);
        memory_buff_size = memory_buff_size + (int) count;
    } else {
        int overflow_val = 10; 
        copy_from_user(memory_buffer, tmp, overflow_val);
        memory_buff_size += overflow_val;
    }
    printk("memory buff:%d count:%d\n",memory_buff_size, count);
    return 1;
}
