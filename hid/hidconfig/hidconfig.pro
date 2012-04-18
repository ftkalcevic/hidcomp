TEMPLATE = app
TARGET = hidconfig
CONFIG += qt precompile_header
include(../build.pri)

CONFIG(debug,debug|release) {
DESTDIR=debug
} else {
DESTDIR=release
}

QT += xml 

DEFINES += QT_XML_LIB USING_PCH
PRECOMPILED_HEADER=stdafx.h

INCLUDEPATH +=  ./generatedfiles
CONFIG(debug,debug|release) {
INCLUDEPATH +=  ./generatedfiles/debug
} else {
INCLUDEPATH +=  ./generatedfiles/release
}

win32:INCLUDEPATH += ../libusb-win32-device-bin-0.1.12.1/include
unix:INCLUDEPATH += ../libusb-1.0.2/libusb ../utility ../usbhid ../hid ../include ../qtcommon

CONFIG(debug,debug|release) {    
    LIBS += -L../hid/debug -lhid -L../usbhid/debug -lusbhid -L../utility/debug -lutility -L../libusb-1.0.2/libusb/.libs -l:libusb-1.0.a -L../qtcommon/debug -lqtcommon
} else {    
    LIBS += -L../hid/release -lhid -L../usbhid/release -lusbhid -L../utility/release -lutility -L../libusb-1.0.2/libusb/.libs -l:libusb-1.0.a -L../qtcommon/release -lqtcommon
}

CONFIG(debug,debug|release) {
PRE_TARGETDEPS = ../hid/debug/libhid.a ../usbhid/debug/libusbhid.a ../utility/debug/libutility.a
} else {
PRE_TARGETDEPS = ../hid/release/libhid.a ../usbhid/release/libusbhid.a ../utility/release/libutility.a
}

DEPENDPATH += .
CONFIG(debug,debug|release) {
MOC_DIR += ./generatedfiles/debug
} else {
MOC_DIR += ./generatedfiles/release
}
CONFIG(debug,debug|release) {
OBJECTS_DIR += debug
} else {
OBJECTS_DIR += release
}
UI_DIR += ./generatedfiles
RCC_DIR += ./generatedfiles

#Include file(s)
include(hidconfig.pri) 

