SUMMARY = "Qt Dashboard UI"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
PR = "r1"
# Use the directory itself, S must match where CMakeLists.txt lands
SRC_URI = "file://aa"

# When using file:// with a directory, files land in WORKDIR directly
S = "${WORKDIR}/aa"

inherit cmake qt6-cmake

DEPENDS = " \
    qtbase \
    qtbase-native \
    qtdeclarative \
    qtdeclarative-native \
    qtserialbus \
"

do_install:append() {
    # Install binary
    install -d ${D}${bindir}
    install -m 0755 ${B}/qt-dashboard-ui-test ${D}${bindir}/dashboard-app

    # Install QML files and assets
    install -d ${D}${datadir}/dashboard
    cp -rf ${S}/*.qml ${D}${datadir}/dashboard/
    cp -rf ${S}/*.png ${D}${datadir}/dashboard/
    cp -rf ${S}/*.json ${D}${datadir}/dashboard/

    # Install vcan setup script
    install -m 0755 ${S}/vcan.sh ${D}${bindir}/setup-vcan

    # Install sim scripts
    install -d ${D}${datadir}/dashboard/sim
    install -m 0755 ${S}/sim/*.py ${D}${datadir}/dashboard/sim/
}

FILES:${PN} = " \
    ${bindir}/dashboard-app \
    ${bindir}/setup-vcan \
    ${datadir}/dashboard \
    ${datadir}/dashboard/sim \
    ${datadir}/dashboard/sim/*.py \
"

RDEPENDS:${PN} = " \
    qtbase \
    qtdeclarative \
    qtserialbus \
    python3 \
"