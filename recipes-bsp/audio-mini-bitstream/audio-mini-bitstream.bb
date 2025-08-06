SUMMARY = "Audio Mini FPGA bitstream for DE10-Nano"
DESCRIPTION = "FPGA bitstream (RBF) files for the Audio Mini hardware design on DE10-Nano"
HOMEPAGE = "https://github.com/ADSD-SoC-FPGA"
BUGTRACKER = "https://github.com/ADSD-SoC-FPGA/Code/issues"
SECTION = "bsp"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${RBF_FILE};md5=199549b38bcbab6928db8fab9a41c5ee"

# Source files
SRC_URI = "file://soc_system.rbf"

# Source directory
S = "${WORKDIR}"
RBF_FILE = "soc_system.rbf"

RBF_LOCATION = "${THISDIR}/files/${RBF_FILE}"

inherit deploy

do_install() {
    # Install RBF file to sysroot for other recipes to use
    install -d ${D}${datadir}/bitstreams
    install -m 0644 ${S}/${RBF_FILE} ${D}${datadir}/bitstreams/${RBF_FILE}
}

do_deploy() {
    # Deploy RBF file to images directory
    if [ "${DE10_NANO_RBF_FILE}" != "" ]; then
        bbwarn "Using custom RBF file: ${DE10_NANO_RBF_FILE}"
        rbf_file="${DE10_NANO_RBF_FILE}"
    else
        rbf_file="${RBF_LOCATION}"
        bbwarn "Using default RBF file: ${rbf_file}"
    fi

    # Copy RBF to deploy directory
    bbwarn "Copying RBF file to deploy directory: ${DEPLOYDIR}/${RBF_FILE}"
    cp ${rbf_file} ${DEPLOYDIR}/${RBF_FILE}
}

do_tftp_deploy() {
    if [ "${DE10_NANO_TFTP_DIR}" != "" ] && [ "${DE10_NANO_DEPLOY_CONFIG}" == "tftp-nfs" ]; then
        bbwarn "Copying RBF file to TFTP directory: ${DE10_NANO_TFTP_DIR}"
        cp -Lf ${DEPLOYDIR}/${RBF_FILE} ${DE10_NANO_TFTP_DIR}/${RBF_FILE}
    fi
}

# Specify the files installed by the recipe
FILES:${PN} = "${datadir}/bitstreams/${RBF_FILE}"

# Make the RBF available to other recipes
PROVIDES = "audio-mini-bitstream"

# Add deploy tasks
addtask deploy before do_build after do_compile
addtask tftp_deploy after do_deploy
do_tftp_deploy[nostamp] = "1"
