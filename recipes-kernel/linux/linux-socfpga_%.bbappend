# Get Directory Path
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10nano-fragment.cfg"

MACHINE_UNDERSCORE = "${@'${MACHINE}'.replace('-', '_')}"
ZIMAGE_PATH = "${WORKDIR}/linux-${MACHINE_UNDERSCORE}-standard-build/arch/arm/boot/${KERNEL_IMAGETYPE}"



# Copy kernel image to TFTP directory if DE10_NANO_TFTP_DIR is set and DE10_NANO_DEPLOY_CONFIG is tftp-nfs
do_deploy:append() {
    if [ "${DE10_NANO_TFTP_DIR}" != "" ] && [ "${DE10_NANO_DEPLOY_CONFIG}" == "tftp-nfs" ]; then
        bbwarn "Copying kernel image to TFTP directory: ${DE10_NANO_TFTP_DIR}"
        cp -rf "${ZIMAGE_PATH}" "${DE10_NANO_TFTP_DIR}/${KERNEL_IMAGETYPE}"
    fi
}