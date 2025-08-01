# Get Directory Path
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10nano-fragment.cfg"
SRC_URI += "file://de10-nano-audio-mini-base.dts"

do_configure[depends] += "virtual/bootloader:do_unpack"

do_configure:prepend() {
    # Use the MACHINE for U-Boot path (matches actual build directory structure)
    vendor_os=$(echo "${TARGET_SYS}" | cut -d'-' -f2-)
    
    # Convert machine name dashes to underscores to match build directory naming
    machine_dir=$(echo "${MACHINE}" | tr '-' '_')
    
    # Dynamically detect which U-Boot provider is being used
    uboot_provider="${@d.getVar('PREFERRED_PROVIDER_virtual/bootloader') or 'u-boot'}"
    uboot_base="${TMPDIR}/work/${machine_dir}-${vendor_os}/${uboot_provider}"
    
    bbplain "Using U-Boot provider: ${uboot_provider}"
    bbplain "Looking for U-Boot work directory at: ${uboot_base}"
    
    # Find the actual U-Boot work directory with version
    uboot_work_dir=$(ls -d $uboot_base/*/git 2>/dev/null | head -1)
    
    if [ -n "$uboot_work_dir" ] && [ -f "$uboot_work_dir/arch/arm/dts/socfpga_cyclone5_de10_nano.dts" ]; then
        # Copy DTS file from U-Boot source to kernel source (clean copy, no path references)
        cp -f "$uboot_work_dir/arch/arm/dts/socfpga_cyclone5_de10_nano.dts" "${S}/arch/arm/boot/dts/"
        cp -f "${BBDIR}/files/de10-nano-audio-mini-base.dts" "${S}/arch/arm/boot/dts/"
        
        # Create dtsi as relative symlink (no absolute path references)
        cd "${S}/arch/arm/boot/dts/" && ln -sf "socfpga_cyclone5_de10_nano.dts" "socfpga_cyclone5_de10_nano.dtsi"
        cd "${S}/arch/arm/boot/dts/" && ln -sf "de10-nano-audio-mini-base.dts" "de10-nano-audio-mini-base.dtsi"
        
        # Also copy the required dtsi files (clean copy, no path references)
        [ -f "$uboot_work_dir/arch/arm/dts/socfpga_cyclone5.dtsi" ] && cp -f "$uboot_work_dir/arch/arm/dts/socfpga_cyclone5.dtsi" "${S}/arch/arm/boot/dts/"
        [ -f "$uboot_work_dir/arch/arm/dts/socfpga-common-u-boot.dtsi" ] && cp -f "$uboot_work_dir/arch/arm/dts/socfpga-common-u-boot.dtsi" "${S}/arch/arm/boot/dts/"
        [ -f "$uboot_work_dir/arch/arm/dts/socfpga.dtsi" ] && cp -f "$uboot_work_dir/arch/arm/dts/socfpga.dtsi" "${S}/arch/arm/boot/dts/"
        
        # Also add it to the Makefile if not already present
        if ! grep -q "socfpga_cyclone5_de10_nano.dtb" "${S}/arch/arm/boot/dts/Makefile"; then
            echo "dtb-\$(CONFIG_ARCH_SOCFPGA) += socfpga_cyclone5_de10_nano.dtb" >> "${S}/arch/arm/boot/dts/Makefile"
        fi
        if ! grep -q "de10-nano-audio-mini-base.dtb" "${S}/arch/arm/boot/dts/Makefile"; then
            echo "dtb-\$(CONFIG_ARCH_SOCFPGA) += de10-nano-audio-mini-base.dtb" >> "${S}/arch/arm/boot/dts/Makefile"
        fi
    else
        bbwarn "U-Boot DTS file NOT FOUND at: $uboot_work_dir/arch/arm/dts/socfpga_cyclone5_de10_nano.dts"
        bbwarn "U-Boot provider: ${uboot_provider}"
        bbwarn "Looked in path $uboot_base"
        bbwarn "Available U-Boot directories: $(ls -d $uboot_base/*/git 2>/dev/null || echo 'NONE')"
        bberror "Cannot proceed: Required U-Boot DTS file missing. Please check your U-Boot source and provider configuration."
    fi

    if [ -z "$DE10_NANO_CUSTOM_DTB" ] && [ -z "$DE10_NANO_CUSTOM_DTS_PATH" ] && [ -z "$DE10_NANO_CUSTOM_DTS"]; then
        bbwarn "Using custom DTB: ${DE10_NANO_CUSTOM_DTS_PATH}/${DE10_NANO_CUSTOM_DTS}"
        cp -f "${DE10_NANO_CUSTOM_DTS_PATH}/${DE10_NANO_CUSTOM_DTS}" "${S}/arch/arm/boot/dts/"
        if ! grep -q "${DE10_NANO_CUSTOM_DTB}" "${S}/arch/arm/boot/dts/Makefile"; then
            echo "dtb-\$(CONFIG_ARCH_SOCFPGA) += ${DE10_NANO_CUSTOM_DTB}" >> "${S}/arch/arm/boot/dts/Makefile"
        fi
    fi

}