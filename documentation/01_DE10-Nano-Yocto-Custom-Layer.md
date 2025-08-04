# DE10-Nano Yocto Custom Layer

## Overview

This guide explains how to set up a custom Yocto layer for the DE10-Nano board, building on the Passthrough example. The main goal is to demonstrate how to layer your recipes on top of existing recipes and images. This assumes you have working Comb Filter hardware, as the layer depends on the Comb Filter hardware design and drivers.

### Creating the Custom Layer

1. Source the Yocto environment.

2. Run the following command to create the custom layer:

```
bitbake-layers create-layer meta-my-audiomini-combfilter
```

You will now notice a new folder called `meta-my-audiomini-combfilter` is set up.

The directory structure will be as follows:

```
ls ./meta-my-audiomini-combfilter

COPYING.MIT  README  conf  recipes-example
```

3. Edit `./meta-my-audiomini-combfilter/conf/layer.conf`.

We need to add a dependency on the `meta-msu-de10-nano` layer. Change:

```
# We have a conf and classes directory, add to BBPATH
BBPATH .= ":${LAYERDIR}"

# We have recipes-* directories, add to BBFILES
BBFILES += "${LAYERDIR}/recipes-*/*/*.bb \
            ${LAYERDIR}/recipes-*/*/*.bbappend"

BBFILE_COLLECTIONS += "meta-my-audiomini-combfilter"
BBFILE_PATTERN_meta-my-audiomini-combfilter = "^${LAYERDIR}/"
BBFILE_PRIORITY_meta-my-audiomini-combfilter = "6"

LAYERDEPENDS_meta-my-audiomini-combfilter = "core"
LAYERSERIES_COMPAT_meta-my-audiomini-combfilter = "scarthgap"
```

to:

```
# We have a conf and classes directory, add to BBPATH
BBPATH .= ":${LAYERDIR}"

# We have recipes-* directories, add to BBFILES
BBFILES += "${LAYERDIR}/recipes-*/*/*.bb \
            ${LAYERDIR}/recipes-*/*/*.bbappend"

BBFILE_COLLECTIONS += "meta-my-audiomini-combfilter"
BBFILE_PATTERN_meta-my-audiomini-combfilter = "^${LAYERDIR}/"
BBFILE_PRIORITY_meta-my-audiomini-combfilter = "6"

LAYERDEPENDS_meta-my-audiomini-combfilter = "core meta-msu-de10-nano"
LAYERSERIES_COMPAT_meta-my-audiomini-combfilter = "scarthgap"
```

*Notice the change* to `LAYERDEPENDS_meta-my-audiomini-combfilter`, adding `meta-msu-de10-nano`.

4. Remove the `recipes-example` folder:

```
rm -rf ./meta-my-audiomini-combfilter/recipes-example
```

5. Create the following folders:

```
mkdir -p ./meta-my-audiomini-combfilter/recipes-bsp/u-boot/files
mkdir -p ./meta-my-audiomini-combfilter/recipes-bsp/device-tree/files
mkdir -p ./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Driver/files
mkdir -p ./meta-my-audiomini-combfilter/recipes-core/Audio-Mini-CombFilter
```

The `u-boot` directory will hold a `.bbappend` to add BSP files for pinmuxing.

The `device-tree` directory will hold a `.bbappend` to edit the device tree blob compiled for the image.

The `Audio-Mini-CombFilter-Driver` directory will contain the driver to interact with the Comb Filter kernel module.

The `Audio-Mini-CombFilter` directory will define the Comb Filter image for the DE10-Nano to boot.

6. **Setting up the Filter Driver**

Create the file `audiomini-combfilter-driver.bb` in `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Driver` and move your source code (`combFilter.c`), `Makefile`, and `Kbuild` into `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Driver/files`.

The `Makefile` should contain (example):

```
obj-m += combfilter.o
combfilter-objs := combFilter.c
```

The `Kbuild` file should define build rules compatible with the kernel build system, typically similar to the `Makefile` or containing specific kernel module configurations.

7. Open the `audiomini-combfilter-driver.bb` file and add the following:

```
SUMMARY = "Driver for the Comb Filter on the Audio Mini"
DESCRIPTION = "Linux kernel module driver for the Comb Filter on the Audio Mini"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "kernel"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://combFilter.c;md5=0fa3ccd1e0f2998dc2ff6dd90003f4a0"

# Dependencies and provides
DEPENDS += "virtual/kernel"
RDEPENDS:${PN} += "audiomini-drivers"
PROVIDES = "audiomini-combfilter-driver"

# Source files
SRC_URI = "file://combFilter.c \
           file://Makefile \
           file://Kbuild"

# Inherit kernel module class
inherit module

# Set the kernel source directory for the Makefile, uses a KDIR variable for the kernel source directory
EXTRA_OEMAKE += "KDIR=${KERNEL_SRC}"

# Source directory
S = "${WORKDIR}"

do_install() {
    # Install the kernel module to the modules directory for where systemd service looks for it
    install -d ${D}${nonarch_base_libdir}/modules
    install -m 0644 ${S}/combFilter.ko ${D}${nonarch_base_libdir}/modules/
}
```

8. **Understanding this Recipe**

This recipe compiles a Linux kernel module (`combFilter.c`) for the Comb Filter on the Audio Mini, specifically tailored for the DE10-Nano board. Below is an explanation of each component:

- **Metadata**  
  - `SUMMARY` and `DESCRIPTION`: Provide a brief and detailed description of the kernel module’s purpose, identifying it as a driver for the Comb Filter.  
  - `HOMEPAGE` and `BUGTRACKER`: Link to the project’s GitHub page and issue tracker for documentation and issue reporting.  
  - `SECTION = "kernel"`: Categorizes the package as a kernel module, appropriate for a driver loaded into the Linux kernel.  
  - `LICENSE` and `LIC_FILES_CHKSUM`: Specify the GPL-3.0 license and verify it with the MD5 checksum of `combFilter.c` (updated to `0fa3ccd1e0f2998dc2ff6dd90003f4a0`).

- **Dependencies and Provides**  
  - `DEPENDS += "virtual/kernel"`: Ensures the kernel headers are available during compilation, providing access to headers like `linux/platform_device.h`.  
  - `RDEPENDS:${PN} += "audiomini-drivers"`: Ensures the `audiomini-drivers` package is available at runtime, likely providing other kernel modules or utilities required by the Comb Filter.  
  - `PROVIDES = "audiomini-combfilter-driver"`: Allows other recipes to refer to this package by this name for dependency resolution.

- **Source Configuration**  
  - `SRC_URI = "file://combFilter.c file://Makefile file://Kbuild"`: Specifies the source file (`combFilter.c`), `Makefile`, and `Kbuild` file, all located in the recipe’s `files/` directory. The `Makefile` defines the kernel module (`combfilter.ko`) built from `combFilter.c`, while the `Kbuild` file provides additional kernel build system rules.  
  - `S = "${WORKDIR}"`: Sets the working directory to Yocto’s default build directory where source files are extracted.

- **Kernel Module Compilation**  
  - `inherit module`: Uses Yocto’s kernel module build system to compile `combFilter.c` into a kernel module (`.ko` file) using the provided `Makefile` and `Kbuild` files. This ensures access to kernel headers and builds the module for the target kernel (version 6.6.37-altera).  
  - `EXTRA_OEMAKE += "KDIR=${KERNEL_SRC}"`: Passes the kernel source directory (`${KERNEL_SRC}`) to the `Makefile` via the `KDIR` variable, ensuring the build system uses the correct kernel source for compilation.

- **Installation**  
  - `do_install`: Customizes the installation process to place the compiled `combFilter.ko` kernel module in `${nonarch_base_libdir}/modules` (typically `/lib/modules`) on the target system. This directory is where a systemd service or other module-loading mechanism expects to find the module. The permissions (`0644`) ensure the module is readable and writable by the owner.

To build the recipe, run:

```
bitbake audiomini-combfilter-driver
```

To include it in your image, add to `local.conf` or your image recipe:

```
IMAGE_INSTALL:append = " audiomini-combfilter-driver"
```

Ensure the `audiomini-drivers` package is available in your layer or another layer (e.g., `meta-msu-de10-nano`). Verify that the device tree (`recipes-bsp/device-tree`) includes a node for the Comb Filter hardware, with a `compatible` string matching the driver’s `platform_driver` in `combFilter.c` (e.g., `compatible = "combfilter";`). If compilation fails, check the log (`/home/night1rider/university/de10-nano/de-10-nano_minimal/build/tmp/work/de10_nano_audio_mini-poky-linux-gnueabi/audiomini-combfilter-driver/1.0/temp/log.do_compile`) for errors. Ensure the kernel configuration supports necessary features (e.g., `CONFIG_PLATFORM_DEVICE` or `CONFIG_SPI`) and that the `Kbuild` and `Makefile` are correctly configured for your module.


9.
