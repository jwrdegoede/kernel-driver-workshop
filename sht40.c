// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Simple skeleton driver for Sensorion SHT4x sensors
 *
 * Copyright (c) 2025 Hans de Goede <hansg@kernel.org>
 */

#include <linux/i2c.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>

struct sht40_data {
	struct i2c_client *client;
};

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
		.name	= "sht40",
	},
	.probe = sht40_probe,
	.id_table = sht40_i2c_id,
};
module_i2c_driver(sht40_driver);

MODULE_DESCRIPTION("Simple skeleton driver for Sensorion SHT4x sensors");
MODULE_AUTHOR("Hans de Goede <hdegoede@redhat.com>");
MODULE_LICENSE("GPL");
