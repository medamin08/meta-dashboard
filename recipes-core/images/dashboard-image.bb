require recipes-core/images/core-image-minimal.bb

SUMMARY = "BBB Qt Dashboard Image"

IMAGE_INSTALL:append = " \
    dashboard-app \
    qtbase \
    qtbase-plugins \
    qtdeclarative \
    qtserialbus \
    python3 \
    can-utils \
    kernel-modules \
    openssh \
    glibc-utils \
    localedef \
    ttf-dejavu-common \
    ttf-dejavu-sans \
    ttf-dejavu-sans-mono \
    fontconfig \
    fontconfig-utils \
"

DISTRO_FEATURES:append = " opengl"
IMAGE_INSTALL:append = " localedef glibc-utils"
IMAGE_LINGUAS = "en-us"
# Point to our wks file
WKS_FILE = "dashboard-image.wks"
WKS_FILE_DEPENDS = ""

# Keep ext4 and tar, drop wic.gz since we defined our own
IMAGE_FSTYPES = "tar.xz ext4"
