# SPDX-License-Identifier: MIT
# Userspace controller for the Comb Filter on the Audio Mini

# -------------------------------------------------------------------------
# Description: Yocto Recipe for Test program for the combFilterProcessor kernel module
#
# This program demonstrates how to interact with the Comb Filter Kernel Module
# by reading and writing to the device file and sysfs attributes.
# Written by Zackery Backman
# -------------------------------------------------------------------------


SUMMARY = "Userspace controller for the Comb Filter on the Audio Mini"
DESCRIPTION = "Userspace application to control the Comb Filter hardware component"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "applications"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://${WORKDIR}/combFilterController.c;beginline=1;endline=18;md5=623f182eb1270fe1c6a43cf50afdeea1"

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