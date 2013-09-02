/* module_driver.c
 *
 *Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
 *
 *This software is licensed under the terms of the GNU General Public
 *License version 2, as published by the Free Software Foundation, and
 *may be copied, distributed, and modified under those terms.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>

#include <linux/fs.h>
#include <linux/mm.h>

#include "ft5x06_ts.h"

extern int init_wrapper(void);
extern void exit_wrapper(void);

#define module_PRINT_ERR     (1U << 0)
#define module_PRINT_WARNING (1U << 1)
#define module_PRINT_INFO    (1U << 2)
#define module_PRINT_DEBUG   (1U << 3)

#ifndef DEFAULT_DEV_NAME
#define DEFAULT_DEV_NAME "ft5x0x_ts"
#endif
#ifndef DEFAULT_DEV_ADAP
#define DEFAULT_DEV_ADAP 0
#endif
#ifndef DEFAULT_DEV_ADDR
#define DEFAULT_DEV_ADDR 0x38
#endif

#define pr_module(debug_level_mask, args...)                    \
    do {                                                        \
        if (debug_mask & module_PRINT_##debug_level_mask) {     \
            printk(KERN_##debug_level_mask "[module_driver] "args);    \
        }                                                       \
    } while (0)

static int debug_mask = module_PRINT_ERR | \
    module_PRINT_INFO  | \
    module_PRINT_WARNING  | module_PRINT_DEBUG ;
/* static int debug_mask = module_PRINT_ERR | \ */
/*     module_PRINT_INFO  | \ */
/*     module_PRINT_WARNING; */
module_param_named(debug_mask, debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP);
static u8 local_device_adap = DEFAULT_DEV_ADAP;
module_param_named(adap, local_device_adap, byte, S_IRUGO | S_IWUSR | S_IWGRP);
MODULE_PARM_DESC(bus, "Set the i2c adapter of device.");
static u8 local_device_addr = DEFAULT_DEV_ADDR;
module_param_named(addr, local_device_addr, byte, S_IRUGO | S_IWUSR | S_IWGRP);
MODULE_PARM_DESC(addr, "Set the address of device.");

/* Board info ---> Start here. */
#define FT5X06_IRQ_GPIO (48)
#define FT5X06_RESET_GPIO (26)
static struct ft5x0x_platform_data ft5x0x_platformdata  =  {
    .x_max    =  480,
    .y_max    =  800,
    .reset    =  FT5X06_RESET_GPIO,
    .irq      =  FT5X06_IRQ_GPIO,
    .irqflags =  IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
};

static struct i2c_board_info ft5x0x_device_info __initdata  =  {
    I2C_BOARD_INFO(DEFAULT_DEV_NAME, DEFAULT_DEV_ADDR),
    .platform_data  =  &ft5x0x_platformdata,
    .irq  =  MSM_GPIO_TO_INT(FT5X06_IRQ_GPIO),
};

static void ft5x06_touchpad_setup(void)
{
    int rc;

    rc  =  gpio_tlmm_config(GPIO_CFG(FT5X06_IRQ_GPIO, 0,
                                     GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
                                     GPIO_CFG_8MA), GPIO_CFG_ENABLE);
    if (rc)
        pr_err("%s: gpio_tlmm_config for %d failed\n",
               __func__, FT5X06_IRQ_GPIO);

    rc  =  gpio_tlmm_config(GPIO_CFG(FT5X06_RESET_GPIO, 0,
                                     GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
                                     GPIO_CFG_8MA), GPIO_CFG_ENABLE);
    if (rc)
        pr_err("%s: gpio_tlmm_config for %d failed\n",
               __func__, FT5X06_RESET_GPIO);

}
/* Board info ---> End here. */

static struct i2c_client *this_client = NULL;
static struct MODULE_DRIVER_INFO {
    struct i2c_board_info *this_device_info;
    void(*prepare_func)(void);
} module_driver_info = {
    .this_device_info = &ft5x0x_device_info,
    .prepare_func = ft5x06_touchpad_setup
};


static int __init module_driver_init(void)
{
    struct i2c_adapter *i2c_adap;

    pr_module(INFO,"Enter in %s\n", __func__);

    /* Add device driver. */
    if(init_wrapper())
    {
        goto error_driver;
    }

    /* Init GPIOs */
    if (module_driver_info.prepare_func)
    {
        (*module_driver_info.prepare_func)();
    }

    /* Add i2c device to platform */
    i2c_adap = i2c_get_adapter(local_device_adap);
    if (NULL == i2c_adap)
    {
        pr_module(ERR, "%s: i2c_get_adapter for %d failed\n", __func__, local_device_adap);
        goto error_adapter;
    }
    if (module_driver_info.this_device_info)
    {
        module_driver_info.this_device_info->addr = local_device_addr;
        this_client = i2c_new_device(i2c_adap, module_driver_info.this_device_info);
        if (NULL == this_client)
        {
            pr_module(ERR, "%s: i2c_new_device for %s failed\n", __func__, module_driver_info.this_device_info->type);
            goto error_device;
        }
    }
    pr_module(INFO, "%s: this_client:%p, addr:%#x\n", __func__, this_client, this_client->addr);
    i2c_put_adapter(i2c_adap);

    return 0;

  error_device:
    i2c_put_adapter(i2c_adap);
  error_adapter:
  error_driver:
    return -1;
}

static void __exit module_driver_exit(void)
{
    pr_module(INFO,"Enter in %s\n", __func__);
    exit_wrapper();
    if (this_client)
    {
        i2c_unregister_device(this_client);
    }
}

module_init(module_driver_init);
module_exit(module_driver_exit);

MODULE_AUTHOR("zhiqiang.xu<zhiqiang.xu@phicomm.com.cn>");
MODULE_DESCRIPTION("i2c device module driver");
MODULE_LICENSE("GPL v2");
