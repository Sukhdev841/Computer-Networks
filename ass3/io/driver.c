#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
 
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
 
int32_t value = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev temp_cdev;
 
static int __init t_driver_init(void);
static void __exit t_driver_exit(void);
static int t_driver_open(struct inode *inode, struct file *file);
static int t_driver_release(struct inode *inode, struct file *file);
static long t_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
 
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .open           = t_driver_open,
        .unlocked_ioctl = t_driver_ioctl,
        .release        = t_driver_release,
};
 
static int t_driver_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}
 
static int t_driver_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}
 
static long t_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        copy_from_user(&value ,(int32_t*) arg, sizeof(value));
                        printk(KERN_INFO "Value = %d\n", value);
                        break;
                case RD_VALUE:
                        copy_to_user((int32_t*) arg, &value, sizeof(value));
                        break;
        }
        return 0;
}
 
 
static int __init t_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "s_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&temp_cdev,&fops);
        temp_cdev.owner = THIS_MODULE;
        temp_cdev.ops = &fops;
 
        /*Adding character device to the system*/
        if((cdev_add(&temp_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            unregister_chrdev_region(dev,1);
            return -1;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"s_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            unregister_chrdev_region(dev,1);
            return -1;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"s_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            class_destroy(dev_class);
            return -1;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;
        
}
 
void __exit t_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&temp_cdev);
        unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(t_driver_init);
module_exit(t_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sukhdev Singh");
MODULE_DESCRIPTION("A simple device driver");
MODULE_VERSION("9.0");