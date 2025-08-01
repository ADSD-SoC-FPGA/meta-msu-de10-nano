SUMMARY = "Extract rootfs to NFS server folder"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

inherit allarch

# The NFS directory on your HOST machine where the rootfs will be extracted
NFS_EXPORT_DIR ?= "/srv/nfs/shared/de10nano"

# Use Yocto variables to determine the source tarball
ROOTFS_TARBALL = "${DEPLOY_DIR_IMAGE}/${IMAGE_BASENAME}-${MACHINE}.tar.bz2"

# --- FAKE INSTALL TASK ---
# This task runs during the build to create files for a package.
# ${D} is the destination directory for packaging.
do_install() {
    # Create the /usr/bin directory in the staging area for the package
    install -d ${D}${bindir}
    # Create an empty dummy file inside it
    touch ${D}${bindir}/nfs-export-dummy-file
}
# --- END FAKE INSTALL ---


# Define the custom task to extract the rootfs
do_extract() {
    bbwarn "Extracting Rootfs to ${NFS_EXPORT_DIR}"

    # Use Yocto variables to determine the source tarball
    local rootfs_tarball="${DEPLOY_DIR_IMAGE}/${IMAGE_BASENAME}-${MACHINE}.tar.bz2"

    # Check if tarball exists
    if [ ! -f "${rootfs_tarball}" ]; then
        bberror "Rootfs tarball not found: ${rootfs_tarball}"
        exit 1
    fi

    # Ensure export directory exists and clear its contents
    install -d ${NFS_EXPORT_DIR}
    rm -rf ${NFS_EXPORT_DIR}/*

    # Extract rootfs
    tar -xjf ${rootfs_tarball} -C ${NFS_EXPORT_DIR}
    bbwarn "Rootfs extraction complete."
}

# Add the task to the build chain, to run after the image is fully create
addtask extract after do_image_complete before do_build