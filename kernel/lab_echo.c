// SPDX-License-Identifier: (MIT OR GPL-2.0-only)
/* Original bounded misc-device example. Build-only: never loaded by CI. */
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

#define LAB_CAPACITY 32
static DEFINE_MUTEX(lab_lock);
static DECLARE_WAIT_QUEUE_HEAD(lab_wait);
static char lab_data[LAB_CAPACITY];
static size_t lab_length;

static ssize_t lab_read(struct file *file, char __user *buffer, size_t count, loff_t *position)
{
    (void)position;
    if(count == 0) return 0;
    for(;;) {
        int rc = mutex_lock_interruptible(&lab_lock);
        if(rc) return rc;
        if(lab_length != 0) {
            size_t size = min(count, lab_length);
            if(copy_to_user(buffer, lab_data, size)) { mutex_unlock(&lab_lock); return -EFAULT; }
            lab_length -= size;
            memmove(lab_data, lab_data + size, lab_length);
            mutex_unlock(&lab_lock);
            return size;
        }
        mutex_unlock(&lab_lock);
        if(file->f_flags & O_NONBLOCK) return -EAGAIN;
        rc = wait_event_interruptible(lab_wait, READ_ONCE(lab_length) != 0);
        if(rc) return rc;
    }
}

static ssize_t lab_write(struct file *file, const char __user *buffer, size_t count, loff_t *position)
{
    char local[LAB_CAPACITY];
    (void)file;
    (void)position;
    if(count == 0) return -EINVAL;
    if(count > sizeof(local)) return -EMSGSIZE;
    if(copy_from_user(local, buffer, count)) return -EFAULT;
    mutex_lock(&lab_lock);
    memcpy(lab_data, local, count);
    lab_length = count;
    mutex_unlock(&lab_lock);
    wake_up_interruptible(&lab_wait);
    return count;
}

static __poll_t lab_poll(struct file *file, poll_table *wait)
{
    __poll_t mask = EPOLLOUT | EPOLLWRNORM;
    poll_wait(file, &lab_wait, wait);
    mutex_lock(&lab_lock);
    if(lab_length != 0) mask |= EPOLLIN | EPOLLRDNORM;
    mutex_unlock(&lab_lock);
    return mask;
}

static const struct file_operations lab_fops = {
    .owner = THIS_MODULE,
    .read = lab_read,
    .write = lab_write,
    .poll = lab_poll,
    .llseek = no_llseek,
};

static struct miscdevice lab_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "arm_linux_lab",
    .fops = &lab_fops,
    .mode = 0600,
};

static int __init lab_init(void) { return misc_register(&lab_misc); }
static void __exit lab_exit(void) { misc_deregister(&lab_misc); }
module_init(lab_init);
module_exit(lab_exit);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Bounded educational misc device, build-only in CI");
