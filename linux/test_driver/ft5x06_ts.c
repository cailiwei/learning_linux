/* drivers/input/touchscreen/ft5x06_ts.c
 *
 * FocalTech ft5x0x TouchScreen driver.
 *
 * Copyright (c) 2010  Focal tech Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/i2c.h>
#include <linux/input.h>
#if defined(MODULE)
#include "ft5x06_ts.h"
#else
#include <linux/i2c/ft5x06_ts.h>
#endif
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/timer.h>

#if defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif

#define FTS_CTL_IIC
#define FTS_APK_DEBUG
#define SYSFS_DEBUG
#ifdef FTS_CTL_IIC
#include "focaltech_ctl.h"
#endif
#ifdef SYSFS_DEBUG
#include "ft5x06_ex_fun.h"
#endif
struct ts_event {
	u16 au16_x[CFG_MAX_TOUCH_POINTS];	/*x coordinate */
	u16 au16_y[CFG_MAX_TOUCH_POINTS];	/*y coordinate */
	u8 au8_touch_event[CFG_MAX_TOUCH_POINTS];	/*touch event:
					0 -- down; 1-- contact; 2 -- contact */
	u8 au8_finger_id[CFG_MAX_TOUCH_POINTS];	/*touch ID */
	u16 pressure;
	u8 touch_point;
};

struct ft5x0x_ts_data {
	unsigned int irq;
	unsigned int x_max;
	unsigned int y_max;
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct ts_event event;
	struct ft5x0x_platform_data *pdata;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
};

#define FTS_POINT_UP		0x01
#define FTS_POINT_DOWN		0x00
#define FTS_POINT_CONTACT	0x02


/*
*ft5x0x_i2c_Read-read data and write data by i2c
*@client: handle of i2c
*@writebuf: Data that will be written to the slave
*@writelen: How many bytes to write
*@readbuf: Where to store data read from slave
*@readlen: How many bytes to read
*
*Returns negative errno, else the number of messages executed
*
*
*/
int ft5x0x_i2c_Read(struct i2c_client *client, char *writebuf,
		    int writelen, char *readbuf, int readlen)
{
	int ret;

	if (writelen > 0) {
		struct i2c_msg msgs[] = {
			{
			 .addr = client->addr,
			 .flags = 0,
			 .len = writelen,
			 .buf = writebuf,
			 },
			{
			 .addr = client->addr,
			 .flags = I2C_M_RD,
			 .len = readlen,
			 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret < 0)
			dev_err(&client->dev, "f%s: i2c read error.\n",
				__func__);
	} else {
		struct i2c_msg msgs[] = {
			{
			 .addr = client->addr,
			 .flags = I2C_M_RD,
			 .len = readlen,
			 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 1);
		if (ret < 0)
			dev_err(&client->dev, "%s:i2c read error.\n", __func__);
	}
	return ret;
}
/*write data by i2c*/
int ft5x0x_i2c_Write(struct i2c_client *client, char *writebuf, int writelen)
{
	int ret;

	struct i2c_msg msg[] = {
		{
		 .addr = client->addr,
		 .flags = 0,
		 .len = writelen,
		 .buf = writebuf,
		 },
	};

	ret = i2c_transfer(client->adapter, msg, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s i2c write error.\n", __func__);

	return ret;
}

/*release the point*/
static void ft5x0x_ts_release(struct ft5x0x_ts_data *data)
{
	input_report_abs(data->input_dev, BTN_TOUCH, 0);
	input_sync(data->input_dev);
	//pr_info("[FT5x06]:%s\n",__func__);
}

/*Read touch point information when the interrupt  is asserted.*/
static int ft5x0x_read_Touchdata(struct ft5x0x_ts_data *data)
{
	struct ts_event *event = &data->event;
	u8 buf[POINT_READ_BUF] = { 0 };
	int ret = -1;
	int i = 0;
	u8 pointid = FT_MAX_ID;

	ret = ft5x0x_i2c_Read(data->client, buf, 1, buf, POINT_READ_BUF);
	if (ret < 0) {
		dev_err(&data->client->dev, "%s read touchdata failed.\n",
			__func__);
		return ret;
	}
	memset(event, 0, sizeof(struct ts_event));

	event->touch_point = 0;
	for (i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
		pointid = (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
		if (pointid >= FT_MAX_ID)
			break;
		else
			event->touch_point++;
		event->au16_x[i] =
		    (s16) (buf[FT_TOUCH_X_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
		    8 | (s16) buf[FT_TOUCH_X_L_POS + FT_TOUCH_STEP * i];
		event->au16_y[i] =
		    (s16) (buf[FT_TOUCH_Y_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
		    8 | (s16) buf[FT_TOUCH_Y_L_POS + FT_TOUCH_STEP * i];
		event->au8_touch_event[i] =
		    buf[FT_TOUCH_EVENT_POS + FT_TOUCH_STEP * i] >> 6;
		event->au8_finger_id[i] =
		    (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
	}

	event->pressure = FT_PRESS;

	return 0;
}


/*Get the virtual key type and report*/
static void report_key(struct ft5x0x_ts_data *data, u16 x, int flag)
{
	int key_type = 0;
	if (x > 90 && x < 130) {
		key_type = KEY_MENU;
	} else if (x > 230 && x < 260) {
		key_type = KEY_HOME;
	} else if (x > 360 && x < 400) {
		key_type = KEY_BACK;
	}
	input_report_key(data->input_dev, key_type, flag);

}
/*
*report the point information
*/
static void ft5x0x_report_value(struct ft5x0x_ts_data *data)
{
	struct ts_event *event = &data->event;
	int i = 0;
	int up_point = 0;
	int touch_point = 0;

	for (i = 0; i < event->touch_point; i++) {
		/* LCD view area */
		if (event->au16_x[i] < data->x_max
		    && event->au16_y[i] < data->y_max) {
			input_report_abs(data->input_dev, ABS_MT_POSITION_X,
					 event->au16_x[i]);
			input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
					 event->au16_y[i]);
			input_report_abs(data->input_dev, ABS_MT_PRESSURE,
					 event->pressure);
			input_report_abs(data->input_dev, ABS_MT_TRACKING_ID,
					 event->au8_finger_id[i]);

			//input_mt_sync(data->input_dev);

			if (event->au8_touch_event[i] == FTS_POINT_DOWN
			    || event->au8_touch_event[i] == FTS_POINT_CONTACT){
				input_report_key(data->input_dev, BTN_TOUCH,
					       	event->pressure);
				//pr_info("!!!Touch DOWN!!!\n");
			}
			else {
				input_report_key(data->input_dev, BTN_TOUCH, 0);
				up_point++;
				//pr_info("!!!Touch UP!!!\n");
			}
			touch_point ++;
		}
		else if (event->au16_y[i] > 800) {	/*pressed on the key area*/
			if (event->au8_touch_event[i] == FTS_POINT_DOWN
			    || event->au8_touch_event[i] == FTS_POINT_CONTACT)
			{
				report_key(data, event->au16_x[i], 1);
				//pr_info("[FT5x06]:report_key---down!!!");
			}
			else if (event->au8_touch_event[i] == FTS_POINT_UP)
			{
				report_key(data, event->au16_x[i], 0);
				//pr_info("[FT5x06]:report_key---up!!!");
			}

		}

		input_mt_sync(data->input_dev);
	}
	input_sync(data->input_dev);

	if (event->touch_point == 0)
		ft5x0x_ts_release(data);
	//pr_info("[FT5x06]:%s:i=%d,event->au16_x[i-1]=%d,event->au16_y[i-1]=%d\n",
	//		__func__, i, event->au16_x[i-1], event->au16_y[i-1]);
}

/*The ft5x0x device will signal the host about TRIGGER_FALLING.
*Processed when the interrupt is asserted.
*/
static irqreturn_t ft5x0x_ts_interrupt(int irq, void *dev_id)
{
	struct ft5x0x_ts_data *ft5x0x_ts = dev_id;
	int ret = 0;
	//pr_info("[FT5x06]:go into %s\n",__func__);
	disable_irq_nosync(ft5x0x_ts->irq);

	ret = ft5x0x_read_Touchdata(ft5x0x_ts);
	if (ret == 0)
		ft5x0x_report_value(ft5x0x_ts);

	enable_irq(ft5x0x_ts->irq);

	return IRQ_HANDLED;
}

static void ft5x06_ts_early_suspend(struct early_suspend *h);
static void ft5x06_ts_late_resume(struct early_suspend *h);

static int ft5x0x_ts_probe(struct i2c_client *client,
                           const struct i2c_device_id *id)
{
    struct ft5x0x_platform_data *pdata =
        (struct ft5x0x_platform_data *)client->dev.platform_data;
    struct ft5x0x_ts_data *ft5x0x_ts;
    struct input_dev *input_dev;
    int err = 0;
    unsigned char uc_reg_value;
    unsigned char uc_reg_addr;

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        err = -ENODEV;
        goto exit_check_functionality_failed;
    }

    ft5x0x_ts = kzalloc(sizeof(struct ft5x0x_ts_data), GFP_KERNEL);

    if (!ft5x0x_ts) {
        err = -ENOMEM;
        goto exit_alloc_data_failed;
    }

    i2c_set_clientdata(client, ft5x0x_ts);
    ft5x0x_ts->irq = client->irq;
    ft5x0x_ts->client = client;
    ft5x0x_ts->pdata = pdata;
    ft5x0x_ts->x_max = pdata->x_max - 1;
    ft5x0x_ts->y_max = pdata->y_max - 1;
#ifdef CONFIG_PM
    err = gpio_request(pdata->reset, "ft5x0x reset");
    if (err < 0) {
        dev_err(&client->dev, "%s:failed to set gpio reset.\n",
                __func__);
        goto exit_request_reset;
    }
#endif
    gpio_set_value(pdata->reset, 0);
    msleep(20);
    gpio_set_value(pdata->reset, 1);
    msleep(100);

    err = request_threaded_irq(client->irq, NULL, ft5x0x_ts_interrupt,
                               pdata->irqflags, client->dev.driver->name,
                               ft5x0x_ts);
    if (err < 0) {
        dev_err(&client->dev, "ft5x0x_probe: request irq failed\n");
        goto exit_irq_request_failed;
    }
    disable_irq(client->irq);

    input_dev = input_allocate_device();
    if (!input_dev) {
        err = -ENOMEM;
        dev_err(&client->dev, "failed to allocate input device\n");
        goto exit_input_dev_alloc_failed;
    }

    ft5x0x_ts->input_dev = input_dev;

    set_bit(ABS_MT_TOUCH_MAJOR, input_dev->absbit);
    set_bit(ABS_MT_POSITION_X, input_dev->absbit);
    set_bit(ABS_MT_POSITION_Y, input_dev->absbit);
    set_bit(ABS_MT_PRESSURE, input_dev->absbit);
    set_bit(BTN_TOUCH, input_dev->keybit);
    set_bit(KEY_MENU, input_dev->keybit);
    set_bit(KEY_BACK, input_dev->keybit);
    set_bit(KEY_HOME, input_dev->keybit);

    input_set_abs_params(input_dev,
                         ABS_MT_POSITION_X, 0, ft5x0x_ts->x_max, 0, 0);
    input_set_abs_params(input_dev,
                         ABS_MT_POSITION_Y, 0, ft5x0x_ts->y_max, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, PRESS_MAX, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, PRESS_MAX, 0, 0);
    input_set_abs_params(input_dev,
                         ABS_MT_TRACKING_ID, 0, CFG_MAX_TOUCH_POINTS, 0, 0);

    set_bit(EV_KEY, input_dev->evbit);
    set_bit(EV_ABS, input_dev->evbit);

    input_dev->name = FT5X0X_NAME;
    err = input_register_device(input_dev);
    if (err) {
        dev_err(&client->dev,
                "ft5x0x_ts_probe: failed to register input device: %s\n",
                dev_name(&client->dev));
        goto exit_input_register_device_failed;
    }
    /*make sure CTP already finish startup process */
    msleep(150);

    /*get some register information */
    uc_reg_addr = FT5x0x_REG_FW_VER;

    if(ft5x0x_i2c_Read(client, &uc_reg_addr, 1, &uc_reg_value, 1) < 0)
    {
        err = -ENOMEM;
        goto exit_input_register_device_failed;
    }
    dev_dbg(&client->dev, "[FTS] Firmware version = 0x%x\n", uc_reg_value);

    uc_reg_addr = FT5x0x_REG_POINT_RATE;
    if(ft5x0x_i2c_Read(client, &uc_reg_addr, 1, &uc_reg_value, 1) < 0)
    {
        err = -ENOMEM;
        goto exit_input_register_device_failed;
    }
    dev_dbg(&client->dev, "[FTS] report rate is %dHz.\n",
            uc_reg_value * 10);

    uc_reg_addr = FT5X0X_REG_THGROUP;
    if(ft5x0x_i2c_Read(client, &uc_reg_addr, 1, &uc_reg_value, 1) < 0)
    {
        err = -ENOMEM;
        goto exit_input_register_device_failed;
    }
    dev_dbg(&client->dev, "[FTS] touch threshold is %d.\n",
            uc_reg_value * 4);
#ifdef SYSFS_DEBUG
    ft5x0x_create_sysfs(client);
#endif
#ifdef FTS_APK_DEBUG
    ft5x0x_create_apk_debug_channel(client);
#endif

#ifdef FTS_CTL_IIC
    if (ft_rw_iic_drv_init(client) < 0)
        dev_err(&client->dev, "%s:[FTS] create fts control iic driver failed\n",
                __func__);
#endif
    enable_irq(client->irq);
#ifdef CONFIG_HAS_EARLYSUSPEND
    ft5x0x_ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    ft5x0x_ts->early_suspend.suspend = ft5x06_ts_early_suspend;
    ft5x0x_ts->early_suspend.resume = ft5x06_ts_late_resume;
    register_early_suspend(&ft5x0x_ts->early_suspend);
#endif
    return 0;

  exit_input_register_device_failed:
    input_free_device(input_dev);

  exit_input_dev_alloc_failed:
    free_irq(client->irq, ft5x0x_ts);
#ifdef CONFIG_PM
  exit_request_reset:
    gpio_free(ft5x0x_ts->pdata->reset);
#endif

  exit_irq_request_failed:
    i2c_set_clientdata(client, NULL);
    kfree(ft5x0x_ts);

  exit_alloc_data_failed:
  exit_check_functionality_failed:
    return err;
}

#ifdef CONFIG_PM
static int ft5x06_suspend(struct i2c_client *client, pm_message_t state)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(client);
	int ret;
	pr_info("[ft5x06] %s \n", __func__);
	disable_irq(data->irq);
    /*Set IC switch to hibernate mode*/
    ret = i2c_smbus_write_byte_data(client, POWER_MODE_ADDR, HIBERNATE);
    if (ret)
        pr_err("i2c write hibernate command error: %d\n", ret);
	/* report finger up when suspend , add by puyan*/
	input_report_key(data->input_dev, BTN_TOUCH, 0);
	input_sync(data->input_dev);

    return 0;
}

static int ft5x06_resume(struct i2c_client *client)
{
    struct ft5x0x_ts_data *data = i2c_get_clientdata(client);
    pr_info("[ft5x06] %s \n", __func__);
    msleep(1);
    /*reset IC,switch to nomal mode*/
    gpio_set_value(data->pdata->reset, 0);
    msleep(20);
    gpio_set_value(data->pdata->reset, 1);
    msleep(300);
    enable_irq(data->irq);
    return 0;
}
#else
#define ft5x0x_ts_suspend	NULL
#define ft5x0x_ts_resume		NULL
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ft5x06_ts_early_suspend(struct early_suspend *h)
{
	struct ft5x0x_ts_data *data = container_of(h, struct ft5x0x_ts_data,
		early_suspend);
	ft5x06_suspend(data->client, PMSG_SUSPEND);
}

static void ft5x06_ts_late_resume(struct early_suspend *h)
{
	struct ft5x0x_ts_data *data = container_of(h, struct ft5x0x_ts_data,
	            early_suspend);

	 ft5x06_resume(data->client);
}
#endif

static int __devexit ft5x0x_ts_remove(struct i2c_client *client)
{
	struct ft5x0x_ts_data *ft5x0x_ts;
	ft5x0x_ts = i2c_get_clientdata(client);
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&ft5x0x_ts->early_suspend);
#endif
	input_unregister_device(ft5x0x_ts->input_dev);
	#ifdef CONFIG_PM
	gpio_free(ft5x0x_ts->pdata->reset);
	#endif
	#ifdef FTS_APK_DEBUG
	ft5x0x_release_apk_debug_channel();
	#endif
	#ifdef SYSFS_DEBUG
	ft5x0x_release_sysfs(client);
	#endif
	#ifdef FTS_CTL_IIC
	ft_rw_iic_drv_exit();
	#endif
	free_irq(client->irq, ft5x0x_ts);
	kfree(ft5x0x_ts);
	i2c_set_clientdata(client, NULL);
	return 0;
}

static const struct i2c_device_id ft5x0x_ts_id[] = {
	{FT5X0X_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, ft5x0x_ts_id);

static struct i2c_driver ft5x0x_ts_driver = {
	.probe = ft5x0x_ts_probe,
	.remove = __devexit_p(ft5x0x_ts_remove),
	.id_table = ft5x0x_ts_id,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend = ft5x06_suspend,
	.resume = ft5x06_resume,
#endif
	.driver = {
		   .name = FT5X0X_NAME,
		   .owner = THIS_MODULE,
		   },
};

static int __init ft5x0x_ts_init(void)
{
	int ret;
	ret = i2c_add_driver(&ft5x0x_ts_driver);
	if (ret) {
		printk(KERN_WARNING "Adding ft5x0x driver failed "
		       "(errno = %d)\n", ret);
	} else {
		pr_info("Successfully added driver %s\n",
			ft5x0x_ts_driver.driver.name);
	}
	return ret;
}

static void __exit ft5x0x_ts_exit(void)
{
	i2c_del_driver(&ft5x0x_ts_driver);
}

#if defined(MODULE)
int init_wrapper(void)
{
    return ft5x0x_ts_init();
}
EXPORT_SYMBOL(init_wrapper);

void exit_wrapper(void)
{
    ft5x0x_ts_exit();
}
EXPORT_SYMBOL(exit_wrapper);
#else
module_init(ft5x0x_ts_init);
module_exit(ft5x0x_ts_exit);

MODULE_AUTHOR("<luowj>");
MODULE_DESCRIPTION("FocalTech ft5x0x TouchScreen driver");
MODULE_LICENSE("GPL");
#endif
