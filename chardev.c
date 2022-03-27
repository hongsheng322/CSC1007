// chardev.c
// A program to read number of characters
// To compile, run makefile by entering "make"

// LIBRARIES
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

// GLOBAL DEFINITIONS

#define DEVICE_NAME "chardev"
#define SUCCESS 0

// FUNCTION PROTOTYPES

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

// GLOBAL VARIABLES

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
 
// This function is called when the module is loaded. Initialise the char dev; called by module_init() on line 110
static int __init chardev_init(void)
{ 
       major = register_chrdev(0, DEVICE_NAME, &fops); // Regster and create char dev; Assign major number to device
       if (major < 0)
       {                              
              printk(KERN_ALERT "CharDev: Module load failed\n"); // Check for error when loading
              return major;
       }
       printk(KERN_INFO "CharDev: Loading Module\n"); // Activity update
       printk(KERN_INFO "CharDev: Assigned Major Number %d\n", major); // Activity update
       cls = class_create(THIS_MODULE, DEVICE_NAME); // create struct class pointer; used in calls to device_create
       device_create(cls, NULL, MKDEV(major,0), NULL, DEVICE_NAME); //create a device driver of DEVICE_NAME with major number
       pr_info("Device created on /dev/%s\n", DEVICE_NAME); // Alternative to printk with KERN_INFO; Update activity
       return SUCCESS; // Return 0; Succesful initialisation
} 
 
// This function is called when the module is removed. Called by module_exit() on line 111
static void __exit chardev_exit(void)
{  
       device_destroy(cls, MKDEV(major,0)); // Unregisters and cleans up device previously created with device_create() on line 59
       class_destroy(cls); // Destroy the struct class pointer created on line 58

       unregister_chrdev(major, DEVICE_NAME); // Unregister char dev created on line 50
       printk(KERN_INFO "CharDev: Removing Module\n"); // Update activity
}

// Called when device is opened from user space
static int dev_open(struct inode *inodep, struct file *filep) // Takes the device from down to up state; Ready for use
{
       printk(KERN_INFO "CharDev device opened\n"); // Update activity
       return SUCCESS; // Return 0; Successful, interfaced prepared to be used
}

// Called when data is sent from user space to kernel
static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset)
{
       writeCount++; // Counter to record number of times function called
       printk(KERN_INFO "CharDev is being written %d times\n", writeCount); // Update activity
       strcpy(msg, buffer); // Copy message from msg to buffer
       printk(KERN_INFO "Message from User: %s \nNo. of char: %d\n", msg, strlen(msg)); // Update activity
       return SUCCESS; // Return 0
}

// Called when device is closed in user space
static int dev_release(struct inode *inodep, struct file *filep)
{
       printk(KERN_INFO "CharDev device closed\n"); // Update activity
       return SUCCESS; // Return 0
}

// Called when data sent from device to user space
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
       readCount++; // Record number of times function called
       printk(KERN_INFO "CharDev is being read %d times\n", readCount); // Update activity
       sizeOfMessage = strlen(msg) + 1; // Length of message, including terminating null
       printk(KERN_INFO "Copying %s size of %d bytes to User Space\n", msg, sizeOfMessage); // Update activity
       errors = copy_to_user(buffer, &msg, sizeOfMessage); // Copy to user program the length of message
       return SUCCESS; // Return 0
}

module_init(chardev_init); // chardev_init function (on line 48) to be called at module insertion time
module_exit(chardev_exit); // chardev_exit function (on line 65) to be called at module removal time
 
MODULE_LICENSE("GPL"); // GNU General Public License; GPL Free Software
MODULE_DESCRIPTION("CharDev Module"); // Describes what the module is
MODULE_AUTHOR("CSC1007 Group 6"); // Let others know who the author of this module is