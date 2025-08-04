SUMMARY = "Device tree for DE10-Nano Audio Mini"
DESCRIPTION = "Custom device tree for the DE10-Nano Audio Mini board with audio codec support"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

inherit devicetree

# Device tree source files from U-Boot
SRC_URI = "git://github.com/u-boot/u-boot.git;protocol=https;branch=master"
SRCREV = "${AUTOREV}"

SRC_URI += "file://de10-nano-audio-mini-base.dts"
SRC_URI += "file://de10-nano-audio-mini-base.dtsi"


# U-Boot dependency to ensure it's built before we extract files  
# dtc-native provides the device tree compiler
DEPENDS = "dtc-native"

# Compatible machine
COMPATIBLE_MACHINE = "de10-nano-audio-mini"
COMPATIBLE_MACHINE:class-native = ".*"

# Files to be compiled
DT_FILES = "de10-nano-audio-mini-base.dts"
BB_DEVICE_TREE_DIR = "${WORKDIR}/git/arch/arm/dts"

# Package name for the device tree
PROVIDES = "virtual/dtb de10-nano-audio-mini-devicetree"


# Copy DTSI files from U-Boot source before compilation
do_configure:prepend() {
    # Copy the DTS files from U-Boot source to our work directory
    cp -f "${BB_DEVICE_TREE_DIR}/socfpga_cyclone5.dtsi" "${WORKDIR}/socfpga_cyclone5.dtsi"
    cp -f "${BB_DEVICE_TREE_DIR}/socfpga-common-u-boot.dtsi" "${WORKDIR}/socfpga-common-u-boot.dtsi"
    cp -f "${BB_DEVICE_TREE_DIR}/socfpga.dtsi" "${WORKDIR}/socfpga.dtsi"
    cp -f "${BB_DEVICE_TREE_DIR}/socfpga_cyclone5_de10_nano.dts" "${WORKDIR}/socfpga_cyclone5_de10_nano.dtsi"


    # Use the sources from U-Boot path, but copy our DTS files to the correct location
    cp -f "${BBDIR}/files/de10-nano-audio-mini-base.dts" "${WORKDIR}/de10-nano-audio-mini-base.dts"
    cp -f "${BBDIR}/files/de10-nano-audio-mini-base.dts" "${WORKDIR}/de10-nano-audio-mini-base.dtsi"
}