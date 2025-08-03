# My first kernel driver workshop

This git repo contains a stub Linux i2c_client driver plus instructions
(this file) for a workshop to write a small simple kernel driver. Attendees
of the workshop will be given an USB (CH341T) to I2C converter + an I2C SHT40
temperature/humidity sensor. The workship involves writing a simple kernel
driver to read the temperature and humidity from this sensor.

## Table of Contents

1. Preparation (#preparation)
2. Basic connectivity and hardware check
3. 

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
5. Build the provided skeleton i2c driver: `cd kernel-driver-workshop; make`
6. Test that inserting the module works: `sudo insmod sht40.ko`
    * If inserting the module results in an error you likely have
      secure-boot enabled. You can temporarily disable Linux' secure-boot
      checks by running: FIXME
