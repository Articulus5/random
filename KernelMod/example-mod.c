// Example Linux module version history
// 0.1 - Access via /proc only
// 0.2 - Added access via /dev
// 0.3 - Added write mutex
// 0.4 - TODO: Add minor numbers and driver parameters (adjust buffer space?)
// 0.5 - TODO: Add ioctl interface
// Weblink: https://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/

#include <linux/init.h>   // Needed for macros
#include <linux/kernel.h> // Needed for pr_info()
#include <linux/module.h> // Needed by all modules

#include <linux/version.h>
#include <linux/string.h> // For memset() and memcpy()

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include <linux/mutex.h>

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stanley Tse");
MODULE_DESCRIPTION("Example kernel module");
MODULE_VERSION("0.3");

#define PROC_NAME   "ex_proc"
#define DEVICE_NAME "ex_char"
#define CLASS_NAME  "example"

#define BUFFER_SIZE 256

static DEFINE_MUTEX(example_mutex);

static struct class*  exampleClass  = NULL;
static struct device* exampleDevice = NULL;
static int majorNumber;

static int     example_fopen (struct inode *, struct file *);
static int     example_fclose(struct inode *, struct file *);
static ssize_t example_fread (struct file *, char *, size_t, loff_t *);
static ssize_t example_fwrite(struct file *, const char *, size_t, loff_t *);

static struct proc_dir_entry* proc_entry;

static int  open_count;
static char data[BUFFER_SIZE] = "Example data text.\n";

static ssize_t example_proc_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
    int data_len = strlen(data);
    int rem_len;

    printk(KERN_INFO "Example: Proc read.");

    if (*offset > 0)
        return 0;

    rem_len = copy_to_user(user_buffer, data, data_len);
    *offset = data_len - rem_len;

    return *offset;
}

static ssize_t example_proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset)
{
    int rem_len;

    if(!mutex_trylock(&example_mutex))
    {
        printk(KERN_WARNING "Example: Buffer is being written to by another process.");
        return -EBUSY;
    }

    printk(KERN_INFO "Example: Proc write.");

    if (count > BUFFER_SIZE - 1)
    {
        printk(KERN_ERR "Example: Proc input too long. Got %lu bytes, %d bytes allowed.", count, BUFFER_SIZE);
	    return -ENOSPC;
    }

    memset(data, 0, BUFFER_SIZE);
    rem_len = copy_from_user(data, user_buffer, count);
    if (rem_len)
    {
        printk(KERN_ERR "Example: Proc data copy incomplete, %d bytes not copied.", rem_len);
        //return -EFAULT;
    }

    printk(KERN_INFO "Example: Data is now %s", data);

    mutex_unlock(&example_mutex);

    return count - rem_len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static struct proc_ops pops = {
    .proc_read = example_proc_read,
    .proc_write = example_proc_write,
};
#else
static struct file_operations pops = {
    .owner = THIS_MODULE,
    .read = example_proc_read,
    .write = example_proc_write,
};
#endif

static int example_fopen(struct inode *inode, struct file *file)
{
    open_count++;
    printk(KERN_INFO "Example: Char device opened. %d handles open.", open_count);

    return 0;
}

static int example_fclose(struct inode *inode, struct file *file)
{
    open_count--;
    printk(KERN_INFO "Example: Char device closed. %d handles open.", open_count);

    return 0;
}

static ssize_t example_fread(struct file *file, char *user_buffer, size_t count, loff_t *offset)
{
    int data_len = strlen(data);
    int read_size = count;
    int rem_len;
    unsigned int index = *offset;

    if (index >= data_len)
    {
        printk(KERN_WARNING "Example: Read offset requested: %u, max offset is %d.", index, data_len - 1);
        read_size = 0; // Attempting to read past the end of the buffer
    }
    else if (index > 0)
    {
        printk(KERN_INFO "Example: Read offset requested: %u.", index);
        data_len -= index;
    }

    if (read_size > data_len)
        read_size = data_len;

    rem_len = copy_to_user(user_buffer, &data[index], read_size);
    *offset += read_size - rem_len;

    printk(KERN_INFO "Example: Char device read. %u bytes requested, %d bytes returned.", (unsigned int)count, read_size - rem_len);

    return read_size - rem_len;
}

static ssize_t example_fwrite(struct file *file, const char *user_buffer, size_t count, loff_t *offset)
{
    int write_size = count;
    int rem_len;
    unsigned int index = *offset;

    if(!mutex_trylock(&example_mutex))
    {
        printk(KERN_WARNING "Example: Buffer is being written to by another process.");
        return -EBUSY;
    }

    printk(KERN_INFO "Example: Char device write. %u bytes incoming.", (unsigned int)count);

    if (index >= BUFFER_SIZE)
    {
        printk(KERN_WARNING "Example: Write offset requested: %u, max offset is %d.", index, BUFFER_SIZE - 1);
        write_size = 0; // Attempting to write past the end of the buffer
    }
    else if (index > 0 && index + write_size >= BUFFER_SIZE)
    {
        printk(KERN_INFO "Example: Write offset requested: %u.", index);
        write_size = BUFFER_SIZE - 1 - index;
    }

    if (write_size > (BUFFER_SIZE - 1 - index))
    {
        printk(KERN_ERR "Example: Char input too long. Got %lu bytes, %d bytes allowed.", count, BUFFER_SIZE - 1);
        write_size = BUFFER_SIZE - 1 - index;
    }

    memset(&data[index], 0, BUFFER_SIZE);
    rem_len = copy_from_user(&data[index], user_buffer, write_size);
    if (rem_len)
    {
        printk(KERN_ERR "Example: Char data copy incomplete, %d of %lu bytes not copied.", rem_len, count);
    }

    printk(KERN_INFO "Example: Data is now %s", data);

    mutex_unlock(&example_mutex);

    return write_size - rem_len;
}

static struct file_operations fops = {
    .open = example_fopen,
    .read = example_fread,
    .write = example_fwrite,
    .release = example_fclose,
};

//int init_module(void)
static int __init example_init(void)
{
    pr_info("Example: Module init.\n");

    proc_entry = proc_create(PROC_NAME, 0666, NULL, &pops);
    if (proc_entry == NULL)
    {
        printk(KERN_ERR "Example: Failed to create proc entry.");
        remove_proc_entry(PROC_NAME, NULL);
        return 0;
    }

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ERR "Example: Failed to register character device.");
        proc_remove(proc_entry);
        return majorNumber;
    }

    exampleClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(exampleClass))
    {
        printk(KERN_ERR "Example: Failed to register device class.");
        proc_remove(proc_entry);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        return PTR_ERR(exampleClass);
    }

    exampleDevice = device_create(exampleClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(exampleDevice))
    {
        printk(KERN_ERR "Example: Failed to create device.");
        proc_remove(proc_entry);
        class_destroy(exampleClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        return PTR_ERR(exampleDevice);
    }

    mutex_init(&example_mutex);
    open_count = 0;

    printk(KERN_INFO "Example: Module loaded.");

    // A non 0 return means init_module failed; module can't be loaded.
    return 0;

}

//void cleanup_module(void)
static void __exit example_exit(void)
{
    pr_info("Example: Module cleanup.\n");

    if (open_count != 0)
        printk(KERN_WARNING "Example: %d handles still open.", open_count);

    proc_remove(proc_entry);

    device_destroy(exampleClass, MKDEV(majorNumber, 0));
    class_unregister(exampleClass);
    class_destroy(exampleClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);

    mutex_destroy(&example_mutex);

    printk(KERN_INFO "Example: Module unloaded.");
}

module_init(example_init);
module_exit(example_exit);