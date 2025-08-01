# Add custom defconfig to U-Boot build
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10_nano_audio_mini_sd_defconfig"



do_configure:prepend() {
    cd "${S}/configs"

    # Force symlink both of our standard defconfigs into the u-boot directory
    ln -sf "${BBDIR}/files/de10_nano_audio_mini_sd_defconfig" "de10_nano_audio_mini_sd_defconfig"
    ln -sf "${BBDIR}/files/de10_nano_audio_mini_tftp_nfs_defconfig" "de10_nano_audio_mini_tftp_nfs_defconfig"

    # Add custom defconfig to U-Boot build if avaliable
    if [ -n "$CUSTOM_UBOOT_CONFIG_PATH" ] && [ -n "$CUSTOM_UBOOT_CONFIG" ]; then
        bbwarn "UBOOT_CONFIG_PATH is set, using config: ${CUSTOM_UBOOT_CONFIG_PATH}/${CUSTOM_UBOOT_CONFIG}"
        ln -sf "${CUSTOM_UBOOT_CONFIG_PATH}/${CUSTOM_UBOOT_CONFIG} ${CUSTOM_UBOOT_CONFIG}"
    else # Check to see if custom defconfig was choosen
        for config in $UBOOT_CONFIG; do
            if [ "$config" = "de10-nano-audio-mini-custom" ]; then
                bberror "Custom Paths not set correctly CUSTOM_UBOOT_CONFIG_PATH[${CUSTOM_UBOOT_CONFIG_PATH}] and CUSTOM_UBOOT_CONFIG[${CUSTOM_UBOOT_CONFIG}]"
                break
            fi
        done
    fi
}