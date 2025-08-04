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
EXTRA_OEMAKE += "KDIR=\${KERNEL_SRC}"

# Source directory
S = "${WORKDIR}"



do_install() {
    # Install the kernel module to the modules directory for where systemd service looks for it
    install -d ${D}${nonarch_base_libdir}/modules
    install -m 0644 ${S}/combFilter.ko ${D}${nonarch_base_libdir}/modules/
}