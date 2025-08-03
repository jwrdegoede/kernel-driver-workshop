# My first kernel driver workshop

Workshop attendees will be given an USB (CH341T) to I2C converter + an I2C SHT40
temperature/humidity sensor. The workship involves writing a simple kernel
driver to read the temperature and humidity from this sensor.

This git repo contains a driver for the CH341T i2c_adapter driver and
a stub Linux i2c_client driver as starting point.

## Table of Contents

1. [Preparation](#preparation)
2. [Smoke Test](#smoke-test)
3. [Serial Number Output](#serial-number-output)

## Preparation

Please make sure your laptop is setup to build kernel modules *before*
joining the workshop by running the following steps:

1. Update your local linux installation to the latest kernel from your
distribution, this is necessary to avoid a version mismatch between
the main and devel kernel packages.
    * Fedora/RHEL: `sudo dnf update 'kernel*'`
    * Debian/Ubuntu: `sudo apt update; sudo apt install linux-image-$(dpkg --print-architecture)`

2. If the above command installed a new kernel, then reboot into the new kernel.
3. Install build tools and the devel package for the kernel which provides
the necessary C headers for building kernel modules:
    * Fedora/RHEL: `sudo dnf install make gcc kernel-devel`
    * Debian/Ubuntu: `sudo apt install build-essential linux-headers-$(uname -r)`
4. Clone this git repo: `git clone https://github.com/jwrdegoede/kernel-driver-workshop.git`
5. Build the provided drivers: `cd kernel-driver-workshop; make`
6. Test that inserting kernel modules works: `sudo insmod i2c-ch341-usb.ko`
    * If inserting the module results in a "Key was rejected by service"
      error, then you likely have secure-boot enabled. You can temporarily
      disable Shim's and thus Linux' secure-boot checks by running:
      `sudo mokutil --disable-validation`. This will ask for a password,
      this password will be used to verify physical presence upon rebooting
      so this can really be anything (keep it simple). After typing
      the password reboot and then in the MOK management screen select:
      "Change Secure Boot state" and then follow the instructions to disable
      secure-boot key validation. After rebooting verify that the insmod
      command now works.
    * You can re-enable secure-boot key validation after the workshop by
      running: `sudo mokutil --enable-validation`

## Smoke Test

Before diving into writing the driver, lets first verify everything works
with a quick sanity check:

1. Run: `cd kernel-driver-workshop && make clean && make && sudo insmod i2c-ch341-usb.ko`
2. Plug in the provided CHT341T USB adapter with the SHT40 sensor attached
3. Run `sudo dmesg | tail -n 5`. This should show something like:
   "i2c i2c-12: I2C bus registered". The "i2c-12" is what we are after, this
   means that the new i2c-adapter has been assigned i2c-bus-number "12".
4. Run `sudo i2cdetect -y <busnr>` replacing busnr with the number from dmesg.
   This should show a device is found at address 0x44
5. Lets manually read the serial-number from the sensor, to do this run:
   `sudo i2ctransfer -y -f <busnr> w1@0x44 0x89; sleep .1; sudo i2ctransfer -y -f <busnr> r6@0x44`
   Note `<busnr>` is used twice there and needs to be replaced in both cases.
   This should result in 6 hexadecimal numbers being printed.
6. Run `sudo insmod sht40.ko`.
   Dmesg output should show a `SHT4x sensor probe success` message.
   Also `ls -l /sys/bus/i2c/devices/i2c-sht40/` output should show a `driver`
   symlink to the sht40 driver indicating that it has bound to the i2c_client.
7. We will be communicating with userspace through sysfs device attributes,
   the provided stub driver already has 2 example sysfs device attributes
   called "hello" and "world". Test these 2 attributes by running:
    * `cat /sys/bus/i2c/devices/i2c-sht40/hello`
    * `cat /sys/bus/i2c/devices/i2c-sht40/world`
  
## Serial Number Output

Now lets add code to the driver to add a "serialno" sysfs attribute. As shown
in the "i2ctransfer" command in the [Smoke Test](#smoke-test) section this can
be done by sending a single byte command with value 0x89 to the sensor, then
sleeping a bit and then reading back 6 bytes. Here is some example code:

```
	char buf[6];
	int ret;

	ret = i2c_smbus_write_byte(data->client, 0xXX);
	if (ret < 0) {
		dev_err(dev, "Write byte error: %d\n", ret);
		return ret;
	}

	fsleep(1 * USEC_PER_MSEC); 

	ret = i2c_transfer_buffer_flags(data->client, buf, 6, I2C_M_RD);
	if (ret != 6) {
		dev_err(dev, "Read bytes error: %d\n", ret);
		return (ret < 0) ? ret : -EIO;
	}
```

As can be seen in the [datasheet](https://sensirion.com/media/documents/1D662E57/67BD83A2/HT_DS_Datasheet_SHT4xI-Digital_1.pdf)
the output of the 0x89 command is 2 sets of each 3 bytes where each 3 byte
set consists of MSB, LSB + CRC8. So to output the serial number the 3th and
6th bytes returned which are CRC bytes should be ignored, e.g. do:

```
	sysfs_emit(buf, "%02x%02x%02x%02x\n", buf[0], buf[1], buf[3], buf[4]);
```

After making modificications to add a 'serialno' sysfs attribute, run
the following commands to test this:

1. `make && sudo rmmod sht40 && sudo insmod sht40.ko`
2. `cat /sys/bus/i2c/devices/i2c-sht40/serialno`
