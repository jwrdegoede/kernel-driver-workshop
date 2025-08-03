// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Simple skeleton driver for Sensorion SHT4x sensors
 *
 * Copyright (c) 2025 Hans de Goede <hansg@kernel.org>
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/sysfs.h>

struct sht40_data {
	struct i2c_client *client;
};

static ssize_t hello_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct sht40_data *data = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%s says: 'Hello'\n", dev_name(&data->client->dev));
}
static DEVICE_ATTR_RO(hello);

static ssize_t world_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "Hello World\n");
}
static DEVICE_ATTR_RO(world);

static struct attribute *sht40_attrs[] = {
	&dev_attr_hello.attr,
	&dev_attr_world.attr,
	NULL
};
ATTRIBUTE_GROUPS(sht40);

static int sht40_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct sht40_data *data;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;

	dev_info(dev, "SHT4x sensor probe success\n");
	i2c_set_clientdata(client, data);
	return 0;
}

static const struct i2c_device_id sht40_i2c_id[] = {
	{ "sht40" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, sht40_i2c_id);

static struct i2c_driver sht40_driver = {
	.driver = {
		.name = "sht40",
		.dev_groups = sht40_groups,
	},
	.probe = sht40_probe,
	.id_table = sht40_i2c_id,
};
module_i2c_driver(sht40_driver);

MODULE_DESCRIPTION("Simple skeleton driver for Sensorion SHT4x sensors");
MODULE_AUTHOR("Hans de Goede <hdegoede@redhat.com>");
MODULE_LICENSE("GPL");
