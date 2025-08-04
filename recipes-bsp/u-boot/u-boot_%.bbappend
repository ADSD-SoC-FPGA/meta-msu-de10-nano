# Add custom defconfig to U-Boot build
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10_nano_audio_mini_sd_defconfig"
SRC_URI += "file://de10-nano-audio-mini-base.env"

BOOTARGS_ENV = "root=/dev/nfs nfsroot=${DE10_NANO_NFS_IP}:${DE10_NANO_NFS_DIR},port=${DE10_NANO_NFS_PORT},nfsvers=3,tcp earlycon ip=${DE10_NANO_STATIC_IP}:${DE10_NANO_NFS_IP}:${DE10_NANO_GATEWAY}:${DE10_NANO_MASK}::${DE10_NANO_ETH_ADAPTER}:off rw console=ttyS0,115200n8"
BOOTCMD_TFTP_NFS = "run get-fpgadata; run load-fpga; run get-dtb; run get-kernel; run bridge-enable-de10nano; run bootnfs"

DEPENDS:append = " de10-nano-audio-mini-devicetree"

VENDOR_DIR = "${S}/board/terasic/de10-nano"
QTS_DIR = "${VENDOR_DIR}/qts"
QTS_FILTER_SCRIPT = "${S}/arch/arm/mach-socfpga/qts-filter.sh"
DE10_NANO_HW_PROJECT_BSP_DIR = "${WORKDIR}/bsp-build"

CV_BSP_GENERATOR_SCRIPT = "${S}/arch/arm/mach-socfpga/cv_bsp_generator/cv_bsp_generator.py"



do_configure:prepend() {
    # Prevent Python from creating bytecode files
    export PYTHONDONTWRITEBYTECODE="1"

    # Use the MACHINE for U-Boot path (matches actual build directory structure)
    vendor_os=$(echo "${TARGET_SYS}" | cut -d'-' -f2-)

    # Convert machine name dashes to underscores to match build directory naming
    machine_dir=$(echo "${MACHINE}" | tr '-' '_')

    # Dynamically detect which device tree provider is being used
    audiomini_dt_provider="${@d.getVar('PREFERRED_PROVIDER_virtual/dtb') or 'de10-nano-audio-mini-devicetree'}"
    audiomini_dt_base="${TMPDIR}/work/${machine_dir}-${vendor_os}/${audiomini_dt_provider}"

    bbplain "Using Device Tree provider: ${audiomini_dt_provider}"
    bbplain "Looking for Device Tree work directory at: ${audiomini_dt_base}"

         # Find the actual device tree work directory with version
     audiomini_dt_work_dir=$(ls -d $audiomini_dt_base/* 2>/dev/null | head -1)
     
     bbplain "Device Tree work directory found: ${audiomini_dt_work_dir}"
     
     if [ -d "${audiomini_dt_work_dir}" ]; then
         # Copy DTS files from device tree recipe work directory
         for dts_file in "${audiomini_dt_work_dir}"/*.dts; do
             if [ -f "$dts_file" ]; then
                 cp -f "$dts_file" "${S}/arch/arm/dts/"
                 bbplain "U-Boot: Copied $(basename $dts_file) from device tree recipe work directory"
             fi
         done
         
         # Also copy any needed DTSI files
         for dtsi_file in "${audiomini_dt_work_dir}"/*.dtsi; do
             if [ -f "$dtsi_file" ]; then
                 cp -f "$dtsi_file" "${S}/arch/arm/dts/"
                 bbplain "U-Boot: Copied $(basename $dtsi_file) from device tree recipe work directory"
             fi
         done
     else
         bberror "Could not find device tree work directory: ${audiomini_dt_work_dir}"
         bberror "Base path searched: ${audiomini_dt_base}"
         bberror "Available directories: $(ls -la ${audiomini_dt_base}/../ 2>/dev/null || echo 'parent directory not found')"
     fi

    cd "${S}/configs"

    # Replace CONFIG_BOOTCOMMAND in the tftp_nfs defconfig with our variable
    sed -i "s|^CONFIG_BOOTCOMMAND=.*|CONFIG_BOOTCOMMAND=\"${BOOTCMD_TFTP_NFS}\"|" "${BBDIR}/files/de10_nano_audio_mini_tftp_nfs_defconfig"
    # Set the default device tree to the custom device tree
    sed -i "s|^CONFIG_DEFAULT_DEVICE_TREE=.*|CONFIG_DEFAULT_DEVICE_TREE=\"${DE10_NANO_CUSTOM_DEVICE_TREE}\"|" "${BBDIR}/files/de10_nano_audio_mini_tftp_nfs_defconfig"
    sed -i "s|^CONFIG_DEFAULT_DEVICE_TREE=.*|CONFIG_DEFAULT_DEVICE_TREE=\"${DE10_NANO_CUSTOM_DEVICE_TREE}\"|" "${BBDIR}/files/de10_nano_audio_mini_sd_defconfig"
    # Set the default fdt file to the custom dtb file
    sed -i "s|^CONFIG_DEFAULT_FDT_FILE=.*|CONFIG_DEFAULT_FDT_FILE=\"${DE10_NANO_CUSTOM_DTB}\"|" "${BBDIR}/files/de10_nano_audio_mini_tftp_nfs_defconfig"
    sed -i "s|^CONFIG_DEFAULT_FDT_FILE=.*|CONFIG_DEFAULT_FDT_FILE=\"${DE10_NANO_CUSTOM_DTB}\"|" "${BBDIR}/files/de10_nano_audio_mini_sd_defconfig"
    
    # Set the environment file to the custom environment file
    cp -f "${BBDIR}/files/de10-nano-audio-mini-base.env" "${VENDOR_DIR}/de10-nano-audio-mini-base.env"
    sed -i "s|^bootargs=.*|bootargs=${BOOTARGS_ENV}|" "${VENDOR_DIR}/de10-nano-audio-mini-base.env"




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


    # Setup BSP
    if [ "${DE10_NANO_HW_PROJECT}" != "" ] && [ "${DE10_NANO_HPS_NAME}" != "" ] && [ "${SOC_EDS_DIR}" != "" ] && [ "${QUARTUS_ROOTDIR}" != "" ]; then
        bbplain "Setting up BSP for ${DE10_NANO_HW_PROJECT}"
        bbplain "BSP directory: ${DE10_NANO_HW_PROJECT_BSP_DIR}"

        # Export Quartus Root Directory
        export QUARTUS_ROOTDIR="${QUARTUS_ROOTDIR}"

        if [ "${DE10_NANO_HW_PROJECT_BSP_DIR}" != "" ]; then
            rm -rf "${DE10_NANO_HW_PROJECT_BSP_DIR}"
        fi
        mkdir -p "${DE10_NANO_HW_PROJECT_BSP_DIR}"

        # Run Embedded Shell
        if [ ! -e "${SOC_EDS_DIR}" ]; then
            bberror "SOC_EDS_DIR is not set"
            exit 1
        fi

        if [ ! -e "${DE10_NANO_HW_PROJECT_HPS_DIR}" ]; then
            bberror "DE10_NANO_HW_PROJECT_HPS_DIR is not set"
            exit 1
        fi

        ${SOC_EDS_DIR}/embedded_command_shell.sh -c bsp-create-settings --type spl --bsp-dir ${DE10_NANO_HW_PROJECT_BSP_DIR} --preloader-settings-dir ${DE10_NANO_HW_PROJECT_HPS_DIR}/${DE10_NANO_HPS_NAME} --settings ${DE10_NANO_HW_PROJECT_BSP_DIR}/settings.bsp

        if [ -d "${QTS_DIR}" ]; then
            rm -rf "${QTS_DIR}"
        fi
        mkdir -p "${QTS_DIR}"

        # Run the QTS filter script to generate the appropriate QTS files
        #${QTS_FILTER_SCRIPT} ${KMACHINE} ${DE10_NANO_HW_PROJECT} ${DE10_NANO_HW_PROJECT_BSP_DIR} ${QTS_DIR}
        python3 -B ${CV_BSP_GENERATOR_SCRIPT} -i ${DE10_NANO_HW_PROJECT_HPS_DIR}/${DE10_NANO_HPS_NAME} -o ${QTS_DIR}
        
        #ln -sf "${DE10_NANO_HW_PROJECT_BSP_DIR}/generated/iocsr_config_cyclone5.h" "${QTS_DIR}/iocsr_config.h"
        #ln -sf "${DE10_NANO_HW_PROJECT_BSP_DIR}/generated/pinmux_config.h" "${QTS_DIR}/pinmux_config.h"
        #ln -sf "${DE10_NANO_HW_PROJECT_BSP_DIR}/generated/pll_config.h" "${QTS_DIR}/pll_config.h"
        #ln -sf "${DE10_NANO_HW_PROJECT_BSP_DIR}/generated/sdram/sdram_config.h" "${QTS_DIR}/sdram_config.h"

        # Replace CONFIG with CFG in all QTS header files for U-Boot 2024.01 compatibility
        #sed -i 's/CONFIG_/CFG_/g' "${QTS_DIR}/iocsr_config.h"
        #sed -i 's/CONFIG_/CFG_/g' "${QTS_DIR}/pinmux_config.h"
        #sed -i 's/CONFIG_/CFG_/g' "${QTS_DIR}/pll_config.h"
        #sed -i 's/CONFIG_/CFG_/g' "${QTS_DIR}/sdram_config.h"

        bbplain "Updated QTS files: replaced CONFIG_ with CFG_ for U-Boot 2024.01 compatibility"

    else
        bbwarn "DE10_NANO_HW_PROJECT, DE10_NANO_HPS_NAME, SOC_EDS_DIR, and QUARTUS_ROOTDIR must be set to use correct Pin Muxing for HW Project"
    fi

    if [ "${DE10_NANO_RBF_FILE}" != "" ]; then
        bbwarn "Using RBF file: ${DE10_NANO_RBF_FILE}"
    else
        bbwarn "Using default RBF file: ${DE10_NANO_RBF_FILE}"
    fi


    # Setup FDT file
    cd "${S}/configs"
    sed -i "s|^CONFIG_DEFAULT_FDT_FILE=.*|CONFIG_DEFAULT_FDT_FILE=\"${DE10_NANO_CUSTOM_DTB}\"|" "${BBDIR}/files/de10_nano_audio_mini_tftp_nfs_defconfig"
    sed -i "s|^CONFIG_DEFAULT_FDT_FILE=.*|CONFIG_DEFAULT_FDT_FILE=\"${DE10_NANO_CUSTOM_DTB}\"|" "${BBDIR}/files/de10_nano_audio_mini_sd_defconfig"



}