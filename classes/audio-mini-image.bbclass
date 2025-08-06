# audio-mini-image.bbclass
# Reusable class for Audio Mini images on DE10-Nano
# Provides package management, NFS deployment, and TFTP integration

# Inherit core-image for standard image tasks
inherit core-image

# Dependencies
DEPENDS += "core-image-minimal virtual/fakeroot-native"

# Package Management Configuration
# Add APT and DPKG package managers to the root filesystem
IMAGE_INSTALL:append = " apt dpkg audiomini-drivers"
# Add Debian package feed URI for additional packages
PACKAGE_FEED_URIS:append = " http://mirror.0x.sg/debian/"
# Set base path for package feeds (RPM format)
PACKAGE_FEED_BASE_PATHS:append = " rpm"
# Define supported architectures for package feeds
PACKAGE_FEED_ARCHS:append = " all armhf"

# Use Debian package format (.deb) for package management
PACKAGE_CLASSES ?= "package_deb"

# Specify output image formats
IMAGE_FSTYPES = "tar.gz ext4 wic"

# NFS deployment variables
ROOTFS_TARBALL = "${DEPLOY_DIR_IMAGE}/${IMAGE_BASENAME}-${MACHINE}.rootfs.tar.gz"
NFS_SERVER_SCRIPT = "${DEPLOY_DIR_IMAGE}/deploy_nfs.sh"

# Custom task to deploy the rootfs tarball via script
do_deploy_nfs() {
    if [ ! -f "${ROOTFS_TARBALL}" ]; then
        bberror "Rootfs tarball not found at ${ROOTFS_TARBALL}"
        exit 1
    fi

    if [ -e "${NFS_SERVER_SCRIPT}" ]; then
        rm -f ${NFS_SERVER_SCRIPT}
    fi

    #Also make a latest .wic copy for easy access
    cp -Lf ${DEPLOY_DIR_IMAGE}/${IMAGE_BASENAME}-${MACHINE}.rootfs.wic ${DEPLOY_DIR_IMAGE}/${IMAGE_BASENAME}-${MACHINE}.latest.wic

    echo "#!/bin/bash" > ${NFS_SERVER_SCRIPT}
    echo "sudo rm -rf ${DE10_NANO_NFS_DIR}/*" >> ${NFS_SERVER_SCRIPT}
    echo "sudo tar --same-owner -xzf ${ROOTFS_TARBALL} -C ${DE10_NANO_NFS_DIR}" >> ${NFS_SERVER_SCRIPT}
    chmod +x ${NFS_SERVER_SCRIPT}

    bbwarn "NFS server script: ${NFS_SERVER_SCRIPT}"
}

# Run after do_image_complete
addtask deploy_nfs after do_image_complete before do_build
do_build[recrdeps] += "do_deploy_nfs"
do_deploy_nfs[depends] += "${PN}:do_image_complete"
do_deploy_nfs[nostamp] = "1"

# Force TFTP deployment tasks from individual components to run
do_deploy_nfs[depends] += "linux-socfpga-lts:do_tftp_deploy"
do_deploy_nfs[depends] += "audio-mini-bitstream:do_tftp_deploy" 
do_deploy_nfs[depends] += "de10-nano-audio-mini-devicetree:do_tftp_deploy"