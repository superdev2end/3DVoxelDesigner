#-------------------------------------------------
#
# Project created by QtCreator 2019-11-11T23:57:16
#
#-------------------------------------------------

QT      += core gui
QT      += 3dcore 3drender 3dinput 3dextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "3D Voxel Designer v1.0"
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        newscenedlg.cpp \
        voxel3dline.cpp \
        voxel3dwidget.cpp \
        voxel3dnode.cpp \
        custom3dwindow.cpp \
        customemeshgeometry.cpp \
        voxelbox.cpp \
        voxelcloudline.cpp \
        voxelcloudnode.cpp \
        voxel3dcone.cpp \
        voxel3dmovingarrows.cpp \
        voxelnodemarker.cpp \
        voxel3dtext.cpp \
    voxeloverlayimage.cpp \
    textureimage.cpp

HEADERS += \
        constants.h \
        mainwindow.h \
        newscenedlg.h \
        voxel3dline.h \
        voxel3dwidget.h \
        voxel3dnode.h \
        custom3dwindow.h \
        custommeshgeometry.h \
        voxelbox.h \
        voxelcloudline.h \
        voxelcloudnode.h \
        voxel3dcone.h \
        voxel3dmovingarrows.h \
        voxelnodemarker.h \
        voxel3dtext.h \
    voxeloverlayimage.h \
    textureimage.h

FORMS += \
        mainwindow.ui \
        newscenedlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
win32:RC_FILE += resource.rc
