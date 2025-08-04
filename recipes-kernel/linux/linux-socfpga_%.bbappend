# Get Directory Path
BBDIR := "${@os.path.dirname(d.getVar('FILE', True))}"

FILESEXTRAPATHS:prepend := "${BBDIR}/files:"
SRC_URI += "file://de10nano-fragment.cfg"