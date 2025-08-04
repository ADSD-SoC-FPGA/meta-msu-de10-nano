SUMMARY = "Linux I2C Device Driver for the TPA6130A2 Headphone Amplifier"
DESCRIPTION = "Linux I2C Device Driver for the TPA6130A2 Headphone Amplifier"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "kernel"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://${S}/audio-mini-drivers.service;md5=b07de196e86f9e826054228a95139d4c"

inherit systemd

SRC_URI = "git://github.com/night1rider/ADSD-SoC-FPGA-Code.git;protocol=https;branch=yocto-audio-mini-fixes"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/examples/passthrough/linux/systemd"

# kernel modules to be installed that need to be built first
DEPENDS += " systemd bash"


# Runtime dependencies - ensures these packages are installed when audiomini-drivers is installed
RDEPENDS:${PN} += " ad1939-audiomini tpa6130a2-audiomini systemd bash"

PROVIDES = "audiomini-drivers"

do_install() {
    # Install the systemd service file
    install -d ${D}/etc/systemd/system
    install -d ${D}/usr/local/bin

    # Install the systemd service file and the script to load the drivers via insmod
    install -m 0644 ${S}/audio-mini-drivers.service ${D}/etc/systemd/system/
    install -m 0755 ${S}/load-audio-mini-drivers.sh ${D}/usr/local/bin/

}

# Must specify the files that are installed by the recipe for yocto to track them correctly
FILES:${PN} += "/etc/systemd/system/audio-mini-drivers.service /usr/local/bin/load-audio-mini-drivers.sh"

# Systemd service configuration
SYSTEMD_SERVICE:${PN} += " audio-mini-drivers.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"