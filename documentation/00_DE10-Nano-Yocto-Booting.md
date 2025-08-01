# DE-10 Nano Yocto

The Guide will teach you how to build an embedded linux system using Yocto and how to compile. The end goal being that you will be able to boot a DE10-Nano from both the SD Card and a TFTP/NFS Server.

## Using Yocto Initially

The first this we are going to do it a generic build and boot of yocto using the de10 Nano. This means building u-boot, the linux kernel, and the rootf using Yocto as the source and toolchain control method.

0. Export the directory of this github repo in your shell and do the [Yocto Project Quick Build](https://docs.yoctoproject.org/5.0.11/brief-yoctoprojectqs/index.html) to ensure all the needed dependancies are setup for Yocto

```
export DE10_WORKSPACE=$(pwd)
```

1. First pull down yocto and the Scarthgap(latest LTS) branch:

```
git clone git@github.com:yoctoproject/poky.git --branch=scarthgap $DE10_WORKSPACE/yocto
```

2. Make a directory for all the Mata-Layer that we will include in the project, and future layers we may want tp pull down

Then pull down the Intel Altera Yocto Layer for their reference designs for specificaly the Scarthgap LTS:

```
mkdir -p $DE10_WORKSPACE/meta-layers
git clone git@github.com:altera-fpga/meta-intel-fpga-refdes.git --branch=scarthgap $DE10_WORKSPACE/meta-layers/meta-intel-fpga-refdes
git clone https://git.yoctoproject.org/meta-intel-fpga --branch=scarthgap $DE10_WORKSPACE/meta-layers/meta-intel-fpga
```

3. Make a project directory and then source the Yocto build enviroment to create the `build` directory for your yocto :

```
mkdir -p $DE10_WORKSPACE/de-10-nano_minimal && cd $DE10_WORKSPACE/de-10-nano_minimal
source $DE10_WORKSPACE/yocto/oe-init-build-env 
```

4. Add the Intel FPGA Meta Layer

Open the file `conf/bblayers.conf` located in `$DE10_WORKSPACE/de-10-nano_minimal/build`

Then change the file from 

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

to

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

*NOTE* You will need to use *YOUR* absolute path the the Intel FPGA Meta Layer *DO NOT COPY MY EXAMPLE 1:1*

5. Edit the file `conf/bblayers.conf` located in `$DE10_WORKSPACE/de-10-nano_minimal/build`

Then At the end of the file add the following:

```
MACHINE = "de10-nano-audio-mini"
DE10_NANO_DEPLOY_CONFIG = "sd"
```

6. Run `bitbake core-image-minimal`

Once the build is completed flash the `$DE10_WORKSPACE/de-10-nano_minimal/build/tmp/deploy/images/cyclone5/core-image-minimal-cyclone5.rootfs.wic` to an SD card.

7. Plug in the SD card to the DE10-Nano and open up the serial terminal.

Then Power the board on, you will notice it will load up Uboot, then the linux kernel and then boot into the Yocto rootfs


## Setting up a NFS Boot

sudo tar --same-owner -xzf -C /srv/nfs/shared/de10nano/

tftpboot ${kernel_addr_r} zImage; tftpboot ${fdt_addr_r} ${fdtfile}; setenv bootargs 'root=/dev/nfs nfsroot=192.168.2.10:/srv/nfs/shared/de10nano,port=2049,nfsvers=3,tcp earlycon ip=192.168.2.20:192.168.2.10:0.0.0.0:255.255.255.0::eth0:off rw console=ttyS0,115200n8'; bootz ${kernel_addr_r} - ${fdt_addr_r}