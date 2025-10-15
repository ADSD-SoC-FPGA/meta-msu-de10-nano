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

**Notice the change** to `LAYERDEPENDS_meta-my-audiomini-combfilter`, adding `meta-msu-de10-nano`.

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

### Kernel Module Driver Recipe

**Location**: `meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule/audiomini-combfilter-driver.bb`

**Purpose**: This recipe compiles and installs the Linux kernel module (`combFilter.c`) that provides the low-level interface to the Audio-Mini CombFilter hardware. The kernel module creates device files and sysfs attributes that userspace applications can use to control the hardware.

Create the file `audiomini-combfilter-driver.bb` in `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule` and move your source code (`combFilter.c`), `Makefile`, and `Kbuild` into `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-KernelModule/files`.

The `Makefile` should contain (example):

```
obj-m += combfilter.o
combfilter-objs := combFilter.c
```

The `Kbuild` file should define build rules compatible with the kernel build system, typically similar to the `Makefile` or containing specific kernel module configurations.

Open the `audiomini-combfilter-driver.bb` file and add the following kernel module recipe:

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

### Userspace Controller Recipe

**Location**: `meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller/audiomini-combfilter-controller.bb`

**Purpose**: This recipe compiles and installs the userspace application that controls the CombFilter kernel module. It provides a high-level interface for configuring and managing the Audio-Mini CombFilter hardware through the kernel module's device files and sysfs interfaces.

Create the file `audiomini-combfilter-controller.bb` in `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller` and move your source code (`combFilterController.c`) and systemd service file (`combFilterController.service`) into `./meta-my-audiomini-combfilter/recipes-audio-mini/Audio-Mini-CombFilter-Controller/files`.

Open the `audiomini-combfilter-controller.bb` file and add the following userspace controller recipe:

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


**Conditional FPGA Bitstream Selection**

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

## Remaining Layer Components

### Custom Image Recipe

**Location**: `meta-my-audiomini-combfilter/recipes-core/image/audio-mini-combfilter.bb`

**Purpose**: This recipe defines a complete bootable image specifically for the CombFilter Audio Mini system. It demonstrates how to inherit functionality from a bbclass and add specific packages.

```bitbake
# meta-msu-de10-nano/recipes-core/images/audio-mini-combfilter.bb
SUMMARY = "Custom image with CombFilter support for DE10-Nano"
LICENSE = "MIT"

# Inherit the common Audio Mini image functionality
inherit audio-mini-image

# Add CombFilter-specific packages
IMAGE_INSTALL:append = " audiomini-combfilter-controller"
```

#### Understanding the Custom Image Recipe

- **Inheritance**: Uses `inherit audio-mini-image` to pull in all base Audio Mini functionality
- **Package Addition**: Automatically includes the CombFilter controller and its dependencies
- **Simplicity**: Minimal recipe that leverages the power of bbclass inheritance
- **Complete System**: Results in a bootable image with all necessary components

The inheritance means this image automatically gets:
- Base Audio Mini drivers (`audiomini-drivers`)
- Package management tools (APT, DPKG)
- NFS deployment scripts
- TFTP integration
- Standard Linux filesystem structure

Building this image is covered in the "Building and Running the CombFilter Image" section below, including the required `local.conf` configuration.

### Device Tree Configuration

**Location**: `meta-my-audiomini-combfilter/recipes-bsp/device-tree/de10-nano-audio-mini-devicetree.bbappend`

**Purpose**: This bbappend modifies the base device tree to include CombFilter-specific hardware definitions, ensuring the kernel recognizes the CombFilter hardware at the correct memory address.

```bitbake
BBDIR_APP := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR_APP}/files:"

SRC_URI += "file://de10nano-audiomini-combfilter.dts"

#override the default DT_FILES variable
DT_FILES = " de10nano-audiomini-combfilter.dts"

do_configure:append() {
    # Use the sources from U-Boot path, but copy our DTS files to the correct location
    cp -f "${BBDIR_APP}/files/de10nano-audiomini-combfilter.dts" "${WORKDIR}/de10nano-audiomini-combfilter.dts"
    cp -f "${BBDIR_APP}/files/de10nano-audiomini-combfilter.dts" "${WORKDIR}/de10nano-audiomini-combfilter.dtsi"
}
```

#### Device Tree Source File

**Location**: `meta-my-audiomini-combfilter/recipes-bsp/device-tree/files/de10nano-audiomini-combfilter.dts`

```dts
// SPDX-License-Identifier: GPL-2.0+
#include "de10-nano-audio-mini-base.dtsi"

/{
    model = "Audio Logic Audio Mini";
    
    ad1939 {
        compatible = "dev,al-ad1939";
    };
    
    tpa613a2 {
        compatible = "dev,al-tpa613a2";
    };
    
    combFilterProcessor_0: combFilterProcessor@ff200000 {
        compatible = "kds,combFilterProcessor";  
        reg = <0xff200000 0x10>; 
    };
};

&spi0{
    status = "okay";
};
```

#### Understanding Device Tree Configuration

- **Hardware Definition**: Defines the CombFilter hardware at memory address `0xff200000`
- **Compatible String**: `"kds,combFilterProcessor"` must match the kernel driver's `platform_driver` registration
- **Memory Mapping**: `reg = <0xff200000 0x10>` defines 16 bytes of memory-mapped registers
- **Base Inheritance**: Includes common Audio Mini hardware definitions via `de10-nano-audio-mini-base.dtsi`
- **SPI Interface**: Enables SPI0 interface required for audio codec communication

The device tree serves as the hardware description that allows the Linux kernel to:
1. Recognize the CombFilter hardware at boot
2. Load the appropriate driver when the compatible string matches
3. Map the correct memory addresses for hardware access
4. Enable required peripherals (SPI, I2C, etc.)

## Understanding Yocto bbclass Files

A **bbclass** file is a shared code module in Yocto that provides reusable functionality across multiple recipes. It's similar to a class in object-oriented programming - it encapsulates common behavior that can be inherited by recipes.

### What is a bbclass?

- **Extension**: `.bbclass`
- **Location**: Stored in `classes/` directories within layers
- **Inheritance**: Recipes inherit bbclass functionality using the `inherit` directive
- **Purpose**: Eliminate code duplication and standardize common operations

### Key Benefits of bbclass Files

1. **Code Reuse**: Write once, use in multiple recipes
2. **Standardization**: Ensure consistent behavior across related recipes
3. **Maintenance**: Update functionality in one place affects all inheriting recipes
4. **Abstraction**: Hide complex operations behind simple interfaces
5. **Modularity**: Separate concerns into logical, reusable components

### Example: audio-mini-image.bbclass

**Location**: `meta-msu-de10-nano/classes/audio-mini-image.bbclass`

This bbclass demonstrates several key concepts:

```bitbake
# audio-mini-image.bbclass
# Reusable class for Audio Mini images on DE10-Nano

# Inherit core-image for standard image tasks
inherit core-image

# Package Management Configuration
IMAGE_INSTALL:append = " apt dpkg audiomini-drivers"
PACKAGE_FEED_URIS:append = " http://mirror.0x.sg/debian/"
PACKAGE_CLASSES ?= "package_deb"

# Specify output image formats
IMAGE_FSTYPES = "tar.gz ext4 wic"

# Custom task to deploy the rootfs via NFS
do_deploy_nfs() {
    # ... implementation details ...
}

# Task dependencies and ordering
addtask deploy_nfs after do_image_complete before do_build
```

#### What This bbclass Provides

1. **Standard Image Base**: Inherits `core-image` for basic Linux functionality
2. **Package Management**: Configures APT/DPKG with Debian repositories
3. **Audio Mini Drivers**: Automatically includes required hardware drivers
4. **Multiple Formats**: Generates `.tar.gz`, `.ext4`, and `.wic` images
5. **NFS Deployment**: Custom task for network filesystem deployment
6. **TFTP Integration**: Coordinates with bootloader components

#### How Recipes Use This bbclass

Any recipe can inherit this functionality:

```bitbake
# In a recipe file (.bb)
inherit audio-mini-image

# The recipe now automatically has:
# - All base image functionality
# - Audio Mini drivers included
# - NFS deployment capability
# - Package management configured
# - Multiple output formats
```

### Common bbclass Types in Yocto

1. **Build System Classes**
   - `cmake.bbclass`: CMake build system support
   - `autotools.bbclass`: GNU Autotools support
   - `kernel.bbclass`: Linux kernel building
   - `module.bbclass`: Kernel module compilation

2. **Package Management Classes**
   - `systemd.bbclass`: Systemd service integration
   - `update-rc.d.bbclass`: SysV init script management
   - `useradd.bbclass`: User account creation

3. **Image Classes**
   - `core-image.bbclass`: Base Linux images
   - `image.bbclass`: General image creation
   - `image-live.bbclass`: Live USB/CD images

4. **Development Classes**
   - `externalsrc.bbclass`: External source trees
   - `devshell.bbclass`: Development shell access
   - `rm_work.bbclass`: Build artifact cleanup

### Creating Custom bbclass Files

When creating a bbclass, consider:

1. **Reusability**: Will multiple recipes benefit from this functionality?
2. **Abstraction**: Does it hide complex operations behind a simple interface?
3. **Configuration**: Are there variables that recipes should be able to override?
4. **Dependencies**: What other classes or packages are required?
5. **Tasks**: Are custom build tasks needed?

#### bbclass Best Practices

```bitbake
# my-custom.bbclass

# Provide default values that recipes can override
MY_CUSTOM_VAR ?= "default_value"

# Use conditional assignment for flexibility
MY_SETTING ??= "fallback_value"

# Create reusable functions
my_custom_function() {
    # Reusable functionality here
}

# Define custom tasks when needed
do_my_custom_task() {
    my_custom_function
}

# Set up task dependencies
addtask my_custom_task after do_compile before do_install
```

### Integration with Custom Layers

The `audio-mini-image.bbclass` demonstrates how custom bbclass files enable:

1. **Layer Consistency**: All Audio Mini images share common functionality
2. **Easy Customization**: New images inherit base functionality and add specifics
3. **Maintenance Efficiency**: Updates to common functionality happen in one place
4. **Rapid Development**: New images require minimal code

For example, creating a new variant is simple:

```bitbake
# audio-mini-special.bb
inherit audio-mini-image

# Add special packages
IMAGE_INSTALL:append = " my-special-package"

# Override settings if needed
IMAGE_FSTYPES = "wic"
```

This approach makes the custom layer highly maintainable and allows for rapid development of new Audio Mini variants while ensuring consistency across all implementations.

### Conclusion

The `meta-my-audiomini-combfilter` layer demonstrates a complete Yocto custom layer implementation with:

- **Modular Architecture**: Separate kernel module and userspace components
- **Intelligent Configuration**: Conditional bitstream selection based on build context
- **Hardware Integration**: Proper device tree configuration for FPGA hardware
- **Reusable Infrastructure**: bbclass-based image construction for consistency
- **Development Efficiency**: Minimal code required for new images through inheritance

The bbclass system provides the foundation for maintainable, scalable embedded Linux development, allowing complex functionality to be encapsulated and reused across multiple projects while maintaining clean separation of concerns.

## Summary

This guide has covered the complete setup of a custom Yocto layer for the DE10-Nano board with Audio-Mini CombFilter support. The implementation includes:

- **Custom layer creation and configuration** with proper dependencies
- **Kernel module driver** for hardware interface and platform device integration
- **Userspace controller application** with systemd integration for automatic startup
- **Intelligent conditional bitstream selection** system that automatically switches FPGA configurations
- **Device tree configuration** for proper hardware recognition and memory mapping
- **Custom image recipe** leveraging bbclass inheritance for rapid development
- **Comprehensive bbclass system** demonstrating reusable component architecture
- **Build and deployment guidance** with required configuration steps

### Key Achievements

The modular approach demonstrates several important embedded Linux development concepts:

1. **Hardware Abstraction**: Clean separation between kernel module (hardware interface) and userspace application (business logic)
2. **Intelligent Configuration**: Conditional bitstream selection eliminates manual configuration overhead
3. **Reusable Infrastructure**: bbclass-based image construction ensures consistency across variants
4. **Scalable Architecture**: Easy addition of new components without disrupting existing functionality
5. **Development Efficiency**: Minimal code required for new images through inheritance patterns

This layer serves as a comprehensive template for creating production-ready embedded Linux systems on FPGA platforms, demonstrating best practices for maintainable, scalable development workflows.

## Building and Running the CombFilter Image

Now that all components have been explained, here's how to build and deploy the complete CombFilter image:

### Required local.conf Configuration

**IMPORTANT:** Before building, you must add the following configuration to your `de-10-nano_minimal/build/conf/local.conf` file:

```bitbake
# Override device tree configuration for CombFilter
DE10_NANO_CUSTOM_DEVICE_TREE = "de10nano-audiomini-combfilter"
DE10_NANO_CUSTOM_DTB = "de10nano-audiomini-combfilter.dtb"
DE10_NANO_CUSTOM_DTS = "de10nano-audiomini-combfilter.dts"
```

Add these lines to the end of your `local.conf` file before building the image. Without this configuration, the image will not use the correct device tree for the CombFilter hardware.

**Note:** This manual configuration requirement will be addressed in future layer updates to make the process more automatic.

### Build Commands

```bash
# Source the Yocto environment
source oe-init-build-env

# Build the CombFilter image
bitbake audio-mini-combfilter
```

The build process will:
1. Compile the CombFilter kernel module
2. Build the userspace controller application
3. Select the appropriate FPGA bitstream automatically
4. Configure the device tree for CombFilter hardware
5. Create a complete bootable image with all components

### Deployment

After a successful build, the image files will be available in `de-10-nano_minimal/build/tmp/deploy/images/de10-nano/` and can be flashed to an SD card or deployed via NFS using the generated deployment scripts.
