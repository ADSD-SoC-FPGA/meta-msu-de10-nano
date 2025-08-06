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
mkdir -p ./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule/files
mkdir -p ./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller/files
mkdir -p ./meta-my-audiomini-combfilter/recipes-core/Audio-Mini-CombFilter
```

The `u-boot` directory will hold a `.bbappend` to add BSP files for pinmuxing.

The `device-tree` directory will hold a `.bbappend` to edit the device tree blob compiled for the image.

The `Audio-Mini-CombFilter-KernelModule` directory will contain the kernel module driver that interfaces with the Comb Filter hardware.

The `Audio-Mini-CombFilter-Controller` directory will contain the userspace application that controls the kernel module.

The `Audio-Mini-CombFilter` directory will define the Comb Filter image for the DE10-Nano to boot.

6. **Setting up the Audio-Mini CombFilter Components**

The Audio-Mini CombFilter implementation consists of two main components that work together to provide a complete software stack for the Audio-Mini CombFilter hardware on the DE10-Nano board:

1. **Kernel Module Driver** (`Audio-Mini-CombFilter-KernelModule`) - The Linux kernel module that interfaces with the hardware
2. **Userspace Controller** (`Audio-Mini-CombFilter-Controller`) - The userspace application that controls the kernel module

## Audio-Mini CombFilter Recipe Structure

The Audio-Mini CombFilter recipes are located in:
```
meta-my-audiomini-combfilter/recipes-audio-mini/
├── Audio-Mini-CombFilter-KernelModule/
│   ├── audiomini-combfilter-driver.bb
│   └── files/
│       ├── combFilter.c
│       ├── Makefile
│       └── Kbuild
└── Audio-Mini-CombFilter-Controller/
    ├── audiomini-combfilter-controller.bb
    └── files/
        ├── combFilterController.c
        └── combFilterController.service
```

### 6.1 Kernel Module Driver Recipe

**Location**: `meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule/audiomini-combfilter-driver.bb`

**Purpose**: This recipe compiles and installs the Linux kernel module (`combFilter.c`) that provides the low-level interface to the Audio-Mini CombFilter hardware. The kernel module creates device files and sysfs attributes that userspace applications can use to control the hardware.

Create the file `audiomini-combfilter-driver.bb` in `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule` and move your source code (`combFilter.c`), `Makefile`, and `Kbuild` into `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule/files`.

The `Makefile` should contain (example):

```
obj-m += combfilter.o
combfilter-objs := combFilter.c
```

The `Kbuild` file should define build rules compatible with the kernel build system, typically similar to the `Makefile` or containing specific kernel module configurations.

7. Open the `audiomini-combfilter-driver.bb` file and add the following kernel module recipe:

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

#### Understanding the Kernel Module Recipe

This recipe compiles a Linux kernel module (`combFilter.c`) for the Comb Filter on the Audio Mini, specifically tailored for the DE10-Nano board. Below is an explanation of each component:

- **Metadata**  
  - `SECTION = "kernel"`: Categorizes this as a kernel module package
  - `LICENSE` and `LIC_FILES_CHKSUM`: Specifies GPL-3.0 license and verifies it with MD5 checksum
  - `HOMEPAGE` and `BUGTRACKER`: Links to the project repository

- **Dependencies**  
  - `DEPENDS += "virtual/kernel"`: Ensures kernel headers are available during compilation
  - `RDEPENDS:${PN} += "audiomini-drivers"`: Runtime dependency on the base audio mini drivers
  - `PROVIDES = "audiomini-combfilter-driver"`: Allows other recipes to depend on this package

- **Source Files**  
  - `combFilter.c`: The main kernel module source code
  - `Makefile`: Build rules for compiling the kernel module
  - `Kbuild`: Kernel build system configuration

- **Build Configuration**  
  - `inherit module`: Uses Yocto's kernel module build system
  - `EXTRA_OEMAKE`: Passes kernel source directory to the Makefile
  - `do_install()`: Custom installation to place the module in `/lib/modules`

To build the kernel module:
```bash
bitbake audiomini-combfilter-driver
```

### 6.2 Userspace Controller Recipe

**Location**: `meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller/audiomini-combfilter-controller.bb`

**Purpose**: This recipe compiles and installs the userspace application that controls the CombFilter kernel module. It provides a high-level interface for configuring and managing the Audio-Mini CombFilter hardware through the kernel module's device files and sysfs interfaces.

Create the file `audiomini-combfilter-controller.bb` in `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller` and move your source code (`combFilterController.c`) and systemd service file (`combFilterController.service`) into `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller/files`.

8. Open the `audiomini-combfilter-controller.bb` file and add the following userspace controller recipe:

```bitbake
# SPDX-License-Identifier: MIT
# Userspace controller for the Comb Filter on the Audio Mini

SUMMARY = "Userspace controller for the Comb Filter on the Audio Mini"
DESCRIPTION = "Userspace application to control the Comb Filter hardware component"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "applications"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://${WORKDIR}/combFilterController.c;beginline=1;endline=18;md5=9617f91e006d68e8a5aadf97e0c58bb6"

# Dependencies
DEPENDS = "glibc"
RDEPENDS:${PN} += "systemd audiomini-combfilter-driver"

# Source files
SRC_URI = "file://combFilterController.c \
           file://combFilterController.service"

# Source directory
S = "${WORKDIR}"

# Inherit systemd class
inherit systemd

# Define the systemd service name
SYSTEMD_SERVICE:${PN} = "combFilterController.service"

# Build the userspace application
do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} -o combFilterController ${S}/combFilterController.c
}

# Install the binary and service file
do_install() {
    # Install the binary to /usr/local/bin
    install -d ${D}/usr/local/bin
    install -m 0755 ${S}/combFilterController ${D}/usr/local/bin/combFilterController

    # Install the systemd service file to /etc/systemd/system
    install -d ${D}${sysconfdir}/systemd/system
    install -m 0644 ${S}/combFilterController.service ${D}${sysconfdir}/systemd/system/combFilterController.service
}

# Specify the files installed by the recipe
FILES:${PN} = "/usr/local/bin/combFilterController \
               ${sysconfdir}/systemd/system/combFilterController.service"

# Enable the systemd service
SYSTEMD_AUTO_ENABLE = "enable"
```

#### Understanding the Userspace Controller Recipe

- **Metadata**
  - `SECTION = "applications"`: Categorizes this as a userspace application
  - `LICENSE` and `LIC_FILES_CHKSUM`: Specifies GPL-3.0 license with checksum verification from the source file

- **Dependencies**
  - `DEPENDS = "glibc"`: Build-time dependency on the C library
  - `RDEPENDS:${PN} += "systemd audiomini-combfilter-driver"`: Runtime dependencies on systemd and the kernel module

- **Source Files**
  - `combFilterController.c`: The main userspace application source code
  - `combFilterController.service`: Systemd service file for automatic startup

- **Systemd Integration**
  - `inherit systemd`: Enables systemd service management
  - `SYSTEMD_SERVICE:${PN}`: Specifies the service file name
  - `SYSTEMD_AUTO_ENABLE = "enable"`: Automatically enables the service on boot

- **Build and Installation**
  - `do_compile()`: Compiles the C application using the cross-compiler
  - `do_install()`: Installs the binary to `/usr/local/bin` and service file to systemd directory
  - `FILES:${PN}`: Explicitly lists files provided by this package

To build the userspace controller:
```bash
bitbake audiomini-combfilter-controller
```

## Building Both Components

To build both the kernel module and userspace controller together, you can build them individually or include them in your image.

### Including in an Image

Add to your `local.conf` or image recipe:
```bitbake
IMAGE_INSTALL:append = " audiomini-combfilter-driver audiomini-combfilter-controller"
```

### Layer Dependencies

Ensure your `meta-my-audiomini-combfilter/conf/layer.conf` includes the dependency on the base layer:

```bitbake
LAYERDEPENDS_meta-my-audiomini-combfilter = "core meta-msu-de10-nano"
```

## Hardware Requirements

These recipes assume you have:

1. **Working CombFilter Hardware**: The FPGA design must include the CombFilter IP core
2. **Device Tree Configuration**: The device tree must include a node for the CombFilter hardware with appropriate compatible string
3. **Base Audio Mini Drivers**: The `audiomini-drivers` package must be available

## Integration with DE10-Nano Image

These recipes are designed to integrate with the DE10-Nano custom image. The kernel module provides the hardware abstraction layer, while the userspace controller offers a high-level API for applications.

The systemd service ensures the controller starts automatically on boot, making the CombFilter functionality available immediately after the system starts.

## Development Workflow

1. **Modify Source Code**: Edit files in the respective `files/` directories
2. **Update Checksums**: If modifying licensed files, update the MD5 checksums in the recipes
3. **Build and Test**: Use `bitbake` to build individual components or the full image
4. **Deploy**: Flash the updated image to the DE10-Nano SD card

This modular approach allows for independent development and testing of the kernel module and userspace components while maintaining a clean separation of concerns between hardware abstraction and application logic.

Ensure the `audiomini-drivers` package is available in your layer or another layer (e.g., `meta-msu-de10-nano`). Verify that the device tree (`recipes-bsp/device-tree`) includes a node for the Comb Filter hardware, with a `compatible` string matching the driver's `platform_driver` in `combFilter.c` (e.g., `compatible = "combfilter";`).


9. **Conditional FPGA Bitstream Selection**

The `meta-my-audiomini-combfilter` layer includes an intelligent bitstream selection system that automatically switches between different FPGA configurations based on the packages being built.

### How the Conditional Bitstream Works

The `audio-mini-bitstream` recipe in the base layer provides a default Audio Mini passthrough bitstream. The CombFilter layer includes a `.bbappend` file that intelligently overrides this bitstream when CombFilter packages are detected in the build.

**Location**: `meta-my-audiomini-combfilter/recipes-bsp/audio-mini-bitstream/audio-mini-bitstream.bbappend`

### Automatic Detection Logic

The bbappend uses a Python anonymous function to check `IMAGE_INSTALL` for CombFilter-related packages:

```python
python __anonymous() {
    import bb
    
    # Get IMAGE_INSTALL variable
    image_install = d.getVar('IMAGE_INSTALL', True) or ''
    
    # Check for CombFilter packages (driver OR controller)
    if 'audiomini-combfilter-driver' in image_install:
        # Use CombFilter bitstream
    elif 'audiomini-combfilter-controller' in image_install:
        # Controller implies driver via RDEPENDS - use CombFilter bitstream
    else:
        # Use default passthrough bitstream
}
```

### Bitstream Selection Behavior

- **Default Build**: Uses standard Audio Mini passthrough bitstream from base layer
- **CombFilter Build**: Automatically switches to CombFilter-specific bitstream when:
  - `audiomini-combfilter-driver` is in `IMAGE_INSTALL`
  - `audiomini-combfilter-controller` is in `IMAGE_INSTALL` (implies driver via RDEPENDS)

### Usage Examples

```bitbake
# Standard Audio Mini build - uses default bitstream
IMAGE_INSTALL:append = " ad1939-audiomini tpa6130a2-audiomini"

# CombFilter build - automatically uses CombFilter bitstream  
IMAGE_INSTALL:append = " audiomini-combfilter-controller"
```

### Build Debug Output

The system provides clear logging during the build process:

```
NOTE: === AUDIO-MINI-BITSTREAM BBAPPEND DEBUG ===
NOTE: IMAGE_INSTALL:  audiomini-combfilter-controller
NOTE: CombFilter check result: True (found audiomini-combfilter-controller in IMAGE_INSTALL)
NOTE: Setting CombFilter-specific variables
NOTE: CombFilter bitstream configuration applied
NOTE: === END DEBUG ===
```

### File Structure

The conditional bitstream system requires this file structure:

```
meta-my-audiomini-combfilter/recipes-bsp/audio-mini-bitstream/
├── audio-mini-bitstream.bbappend          # Conditional logic
└── files/
    └── soc_system.rbf                     # CombFilter-specific bitstream
```

10. **Summary**

This guide has covered the complete setup of a custom Yocto layer for the DE10-Nano board with Audio-Mini CombFilter support. The implementation includes:

- Custom layer creation and configuration
- Kernel module driver for hardware interface
- Userspace controller application with systemd integration
- Intelligent conditional bitstream selection system
- Comprehensive build and deployment guidance

The modular approach separates hardware abstraction (kernel module) from application logic (userspace controller), while the conditional bitstream system ensures the correct FPGA configuration is automatically deployed based on the packages being built. This enables independent development and testing while maintaining clean interfaces between components.
