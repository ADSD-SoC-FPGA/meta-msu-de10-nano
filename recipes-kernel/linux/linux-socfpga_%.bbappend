# Get Directory Path
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10nano-fragment.cfg"

MACHINE_UNDERSCORE = "${@'${MACHINE}'.replace('-', '_')}"


# Copy kernel image to TFTP directory if DE10_NANO_TFTP_DIR is set and DE10_NANO_DEPLOY_CONFIG is tftp-nfs
do_tftp_deploy() {
    if [ "${DE10_NANO_TFTP_DIR}" != "" ] && [ "${DE10_NANO_DEPLOY_CONFIG}" == "tftp-nfs" ]; then
        bbwarn "Copying kernel image to TFTP directory: ${DE10_NANO_TFTP_DIR}"
        if [ -f "${DEPLOYDIR}/zImage" ]; then
            cp -Lf "${DEPLOYDIR}/zImage" "${DE10_NANO_TFTP_DIR}/zImage"
            touch "${DE10_NANO_TFTP_DIR}/zImage"
        else
            bberror "Kernel image not found: ${DEPLOYDIR}/zImage"
        fi
    fi
}

# Add the tftp_deploy task to run after deploy
addtask tftp_deploy after do_deploy
do_tftp_deploy[nostamp] = "1"