# Debug and conditionally override variables based on CombFilter driver presence
BBDIR_APP := "${@os.path.dirname(d.getVar('FILE', True))}"
BBFILE_APP := "soc_system.rbf"

python __anonymous() {
    import bb
    import os
    
    bb.note("=== AUDIO-MINI-BITSTREAM BBAPPEND DEBUG ===")
    bb.note(f"Recipe name: {d.getVar('PN')}")
    bb.note(f"Recipe path: {d.getVar('FILE')}")
    
    # Check build targets - look at command line or environment
    build_targets = os.environ.get('BB_BUILD_TARGETS', '')
    bb.note(f"BB_BUILD_TARGETS: {build_targets}")
    
    # Check for CombFilter context
    combfilter_found = False
    reason = "not found"
    
    # Check if we're building audio-mini-combfilter
    if 'audio-mini-combfilter' in build_targets:
        combfilter_found = True
        reason = "found audio-mini-combfilter in build targets"
    # Always use combfilter bitstream if this bbappend is active
    else:
        combfilter_found = True
        reason = "bbappend from meta-my-audiomini-combfilter layer is active"
    
    bb.note(f"CombFilter check result: {combfilter_found} ({reason})")
    
    if combfilter_found:
        bb.note("Setting CombFilter-specific variables")
        d.setVar('LIC_FILES_CHKSUM', 'file://${BBDIR_APP}/files/${BBFILE_APP};md5=5ea2359462f7156694af479c5c06635c')
        d.setVar('RBF_FILE', '${BBFILE_APP}')
        d.setVar('SRC_URI', 'file://${BBDIR_APP}/files/${BBFILE_APP}')
        d.setVar('RBF_LOCATION', '${BBDIR_APP}/files/${BBFILE_APP}')
        bb.note("CombFilter bitstream configuration applied")
    else:
        bb.note("Using default bitstream configuration")
    
    bb.note("=== END DEBUG ===")
}