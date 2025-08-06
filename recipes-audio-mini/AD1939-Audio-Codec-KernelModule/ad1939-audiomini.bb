SUMMARY = "Linux SPI Device Driver for the AD1939 Audio Codec"
DESCRIPTION = "Linux SPI Device Driver for the AD1939 Audio Codec"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "kernel"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://${S}/ad1939.c;md5=aa5c2f1d3aba46aa89e8f3613fa7d4b8"

inherit module

SRC_URI = "git://github.com/night1rider/ADSD-SoC-FPGA-Code.git;protocol=https;branch=yocto-audio-mini-fixes"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/examples/passthrough/linux/ad1939"

DEPENDS += "virtual/kernel"
RDEPENDS:${PN} += "audio-mini-bitstream"

PROVIDES = "ad1939-audiomini"

# Set the kernel source directory for the Makefile, uses a KDIR variable for the kernel source directory
EXTRA_OEMAKE += "KDIR=\${KERNEL_SRC}"


do_install() {
    # Install the kernel module to the modules directory for where systemd service looks for it
    install -d ${D}${nonarch_base_libdir}/modules
    install -m 0644 ${S}/ad1939.ko ${D}${nonarch_base_libdir}/modules/
}