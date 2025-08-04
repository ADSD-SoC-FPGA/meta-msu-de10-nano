SUMMARY = "Linux I2C Device Driver for the TPA6130A2 Headphone Amplifier"
DESCRIPTION = "Linux I2C Device Driver for the TPA6130A2 Headphone Amplifier"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "kernel"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://${S}/tpa613a2.c;md5=df9921522114b92885325c5f0fccd500"

inherit module

SRC_URI = "git://github.com/night1rider/ADSD-SoC-FPGA-Code.git;protocol=https;branch=yocto-audio-mini-fixes"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/examples/passthrough/linux/tpa613a2"

DEPENDS += "virtual/kernel"

PROVIDES = "tpa6130a2-audiomini"

# Set the kernel source directory for the Makefile, uses a KDIR variable for the kernel source directory
EXTRA_OEMAKE += "KDIR=\${KERNEL_SRC}"


do_install() {
    # Install the kernel module to the modules directory for where systemd service looks for it
    install -d ${D}${nonarch_base_libdir}/modules
    install -m 0644 ${S}/tpa613a2.ko ${D}${nonarch_base_libdir}/modules/
}