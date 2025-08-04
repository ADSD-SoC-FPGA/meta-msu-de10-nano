BBDIR_APP := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR_APP}/files:"

SRC_URI += "file://de10nano-audiomini-combfilter.dts"

#override the default DT_FILES variable
DT_FILES = "de10nano-audiomini-combfilter.dts"

do_configure:append() {
    # Use the sources from U-Boot path, but copy our DTS files to the correct location
    cp -f "${BBDIR_APP}/files/de10nano-audiomini-combfilter.dts" "${WORKDIR}/de10nano-audiomini-combfilter.dts"
    cp -f "${BBDIR_APP}/files/de10nano-audiomini-combfilter.dts" "${WORKDIR}/de10nano-audiomini-combfilter.dtsi"
}