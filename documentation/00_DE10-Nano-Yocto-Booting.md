# DE-10 Nano Yocto

This guide will teach you how to build an embedded Linux system using Yocto and how to compile it. The end goal is that you will be able to boot a DE10-Nano from both the SD card and a TFTP/NFS server.

## Using Yocto Initially

The first thing we are going to do is a generic build and boot of Yocto using the DE10-Nano. This means building U-Boot, the Linux kernel, and the rootfs using Yocto as the source and toolchain control method.

0. Export the directory of this GitHub repo in your shell and do the [Yocto Project Quick Build](https://docs.yoctoproject.org/5.0.11/brief-yoctoprojectqs/index.html) to ensure all the needed dependencies are set up for Yocto

```
export DE10_WORKSPACE=$(pwd)
```

1. First pull down yocto and the Scarthgap(latest LTS) branch:

```
git clone git@github.com:yoctoproject/poky.git --branch=scarthgap $DE10_WORKSPACE/yocto
```

2. Make a directory for all the Meta-Layers that we will include in the project, and future layers we may want to pull down

Then pull down the Intel Altera Yocto Layer for their reference designs for specifically the Scarthgap LTS:

```
mkdir -p $DE10_WORKSPACE/meta-layers
git clone https://github.com/night1rider/meta-msu-de10-nano.git
git clone https://git.yoctoproject.org/meta-intel-fpga --branch=scarthgap $DE10_WORKSPACE/meta-layers/meta-intel-fpga
```

3. Make a project directory and then source the Yocto build environment to create the `build` directory for your Yocto:

```
mkdir -p $DE10_WORKSPACE/de-10-nano_minimal && cd $DE10_WORKSPACE/de-10-nano_minimal
source $DE10_WORKSPACE/yocto/oe-init-build-env 
```

4. Add the Intel FPGA Meta Layer

Open the file `conf/bblayers.conf` located in `$DE10_WORKSPACE/de-10-nano_minimal/build`

Then change the file from:

```
# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
# changes incompatibly
POKY_BBLAYERS_CONF_VERSION = "2"

BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \
  /home/night1rider/university/de10-nano/yocto/meta \
  /home/night1rider/university/de10-nano/yocto/meta-poky \
  /home/night1rider/university/de10-nano/yocto/meta-yocto-bsp \
  "
```

to:

```
# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
# changes incompatibly
POKY_BBLAYERS_CONF_VERSION = "2"

BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \
  /home/night1rider/university/de10-nano/yocto/meta \
  /home/night1rider/university/de10-nano/yocto/meta-poky \
  /home/night1rider/university/de10-nano/meta-layers/meta-intel-fpga \
  /home/night1rider/university/de10-nano/meta-layers/meta-msu-de10-nano \
  "
```

**NOTE:** You will need to use *YOUR* absolute path to the Intel FPGA Meta Layer. *DO NOT COPY MY EXAMPLE 1:1*

5. Edit the file `conf/local.conf` located in `$DE10_WORKSPACE/de-10-nano_minimal/build`

Then at the end of the file add the following:

```
MACHINE = "de10-nano-audio-mini"
DE10_NANO_DEPLOY_CONFIG = "sd"
```

6. Run `bitbake audio-mini-passthrough`

Once the build is completed, flash the `$DE10_WORKSPACE/de-10-nano_minimal/build/tmp/deploy/images/cyclone5/core-image-minimal-cyclone5.rootfs.wic` to an SD card.

7. Plug in the SD card to the DE10-Nano and open up the serial terminal.

Then power the board on. You will notice it will load up U-Boot, then the Linux kernel, and then boot into the Yocto rootfs.


## Setting up a NFS Boot

1. Run the Script `setup_servers.sh` located in the `tools` directory:

```
sudo ./tools/setup_servers.sh --help
```

This will set up the NFS and TFTP server for the given interface/MAC address of a device and the IP.

2. Switch `DE10_NANO_DEPLOY_CONFIG = "sd"` to `DE10_NANO_DEPLOY_CONFIG = "tftp-nfs"`

3. Look inside the `conf/machine/de10-nano-audio-mini.conf` to determine if there is any option such as IPs, directories, etc. that you want to change. *DO NOT EDIT THAT FILE*, make all changes in the `local.conf` for Yocto.

4. Run `bitbake audio-mini-passthrough`
