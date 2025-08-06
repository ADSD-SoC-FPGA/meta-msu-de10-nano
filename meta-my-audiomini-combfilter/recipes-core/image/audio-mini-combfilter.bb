# meta-msu-de10-nano/recipes-core/images/audio-mini-combfilter.bb
SUMMARY = "Custom image with CombFilter support for DE10-Nano"
LICENSE = "MIT"

# Inherit the common Audio Mini image functionality
inherit audio-mini-image

# Add CombFilter-specific packages
IMAGE_INSTALL:append = " audiomini-combfilter-controller"

