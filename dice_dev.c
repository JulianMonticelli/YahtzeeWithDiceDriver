/*
 * dice minimal kernel module - /dev version
 *
 * Julian Monticlli <jmm337@pitt.edu>
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>
#include <asm/uaccess.h>

unsigned char get_random_byte(int max);

static ssize_t dice_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	int len = 0;
	/*
	 * Until our streamlength is satisfied, keep calling for random bytes
	 */
	while(count > len) {
		*buf = get_random_byte(6);
		buf = buf + 1; // pointer arithmetic
		len++; // increase len
	}
	
	*ppos = len;

	return len;
}

unsigned char get_random_byte(int max) {
	unsigned char c;
	get_random_bytes(&c, 1);
	return ((c%max) + 1); // mod 6 will be 0-5, so add 1
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/dice.
	 */
	"dice",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	/*
	 * Create the "dice" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/dice device using
	 * the default rules.
	 */
	ret = misc_register(&dice_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register dice misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julian Monticelli <jmm337@pitt.edu>");
MODULE_DESCRIPTION("dice minimal module");
MODULE_VERSION("dev");
