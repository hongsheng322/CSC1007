// chardev.c;
// A simple test of kernel module
// To compile, run makefile by entering "make"
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/poll.h>
#include <linux/cdev.h>

#define DEVICE_NAME "chardev"
#define SUCCESS 0

//PROTOTYPES

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

//GLOBAL VARIABLES

static struct file_operations fops = {
       .open = dev_open,
       .read = dev_read,
       .write = dev_write,
       .release = dev_release,
};

static int major;
static int writeCount, readCount;
static char msg[100];
static int sizeOfMessage;
static int errors;
static struct class *cls;
 
/* This function is called when the module is loaded. */ 
static int __init chardev_init(void)
{ 
       major = register_chrdev(0, DEVICE_NAME, &fops);
       if (major < 0)
       {                              
              printk(KERN_ALERT "CharDev: Module load failed\n");
              return major;
       }
       printk(KERN_INFO "CharDev: Loading Module\n"); 
       printk(KERN_INFO "CharDev: Assigned Major Number %d\n", major);
       cls = class_create(THIS_MODULE, DEVICE_NAME);
       device_create(cls, NULL, MKDEV(major,0), NULL, DEVICE_NAME); //create a device driver of DEVICE_NAME with major number
       pr_info("Device created on /dev/%s\n", DEVICE_NAME);
       return SUCCESS; 
} 
 
/* This function is called when the module is removed. */ 
static void __exit chardev_exit(void)
{  
       device_destroy(cls, MKDEV(major,0));
       class_destroy(cls);

       unregister_chrdev(major, DEVICE_NAME);
       printk(KERN_INFO "CharDev: Removing Module\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
       printk(KERN_INFO "CharDev device opened\n");
       return SUCCESS;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset)
{
       writeCount++;
       printk(KERN_INFO "CharDev is being written %d times\n", writeCount);
       strcpy(msg, buffer);
       printk(KERN_INFO "Message from User: %s \nNo. of char: %d\n", msg, strlen(msg));
       return SUCCESS;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
       printk(KERN_INFO "CharDev device closed\n");
       return SUCCESS;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
       readCount++;
       printk(KERN_INFO "CharDev is being read %d times\n", readCount);
       sizeOfMessage = strlen(msg) + 1; //include terminating null
       printk(KERN_INFO "Copying %s size of %d bytes to User Space\n", msg, sizeOfMessage);
       errors = copy_to_user(buffer, &msg, sizeOfMessage); //copy to user program the length of message
       return SUCCESS;
}

module_init(chardev_init);
module_exit(chardev_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CharDev Module");
MODULE_AUTHOR("CSC1007 Group 6");