# Add custom defconfig to U-Boot build
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10_nano_audio_mini_sd_defconfig"
#SRC_URI += "file://0001-add-extra-env-settings.patch"


#KERNEL_ADDR_STANDIN = "0x01000000"
#FDT_ADDR_STANDIN = "0x02000000"
#FDT_FILE_STANDIN = "socfpga_cyclone5_de10_nano.dtb"

KERNEL_ADDR_STANDIN = "\${kernel_addr_r}"
FDT_ADDR_STANDIN = "\${fdt_addr_r}"
FDT_FILE_STANDIN = "\${fdtfile}"

BOOTCMD_TFTP_NFS = "tftpboot ${KERNEL_ADDR_STANDIN} zImage; tftpboot ${FDT_ADDR_STANDIN} ${FDT_FILE_STANDIN}; setenv bootargs 'root=/dev/nfs nfsroot=${DE10_NANO_NFS_IP}:${DE10_NANO_NFS_DIR},port=${DE10_NANO_NFS_PORT},nfsvers=3,tcp earlycon ip=${DE10_NANO_STATIC_IP}:${DE10_NANO_NFS_IP}:${DE10_NANO_GATEWAY}:${DE10_NANO_MASK}::${DE10_NANO_ETH_ADAPTER}:off rw console=ttyS0,115200n8'; bootz ${KERNEL_ADDR_STANDIN} - ${FDT_ADDR_STANDIN}"




do_configure:prepend() {
    cd "${S}/configs"

    # Replace CONFIG_BOOTCOMMAND in the tftp_nfs defconfig with our variable
    sed -i "s|^CONFIG_BOOTCOMMAND=.*|CONFIG_BOOTCOMMAND=\"${BOOTCMD_TFTP_NFS}\"|" "${BBDIR}/files/de10_nano_audio_mini_tftp_nfs_defconfig"
    
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