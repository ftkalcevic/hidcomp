TEMPLATE = app
TARGET = hidcomp
CONFIG += qt precompiled_header
include(../build.pro)

CONFIG(debug,debug|release) {
DESTDIR=debug
} else {
DESTDIR=release
}

QT += xml
QT -= gui

DEFINES += QT_XML_LIB USING_PCH
PRECOMPILED_HEADER=stdafx.h

INCLUDEPATH +=  ./generatedfiles
CONFIG(debug,debug|release) {
INCLUDEPATH +=  ./generatedfiles/debug
} else {
INCLUDEPATH +=  ./generatedfiles/release
}

win32:INCLUDEPATH += ../libusb-win32-device-bin-0.1.12.1/include
unix:INCLUDEPATH += ../utility ../usbhid ../hid ../include ../../../emc2.core/emc2-dev/include ../libusb-1.0.2/libusb


CONFIG(debug,debug|release) {
unix:LIBS += -L../hid/debug -lhid -L../usbhid/debug -lusbhid -L../utility/debug -lutility -L../../../emc2.core/src/emc2/lib -lemchal -lemc -lnml -lemcini  -L../libusb-1.0.2/libusb/.libs -l:libusb-1.0.a 
} else {
unix:LIBS += -L../hid/release -lhid -L../usbhid/release -lusbhid -L../utility/release -lutility -L../../../emc2.core/src/emc2/lib -lemchal -lemc -lnml -lemcini  -L../libusb-1.0.2/libusb/.libs -l:libusb-1.0.a 
}

debug:PRE_TARGETDEPS = ../hid/debug/libhid.a ../usbhid/debug/libusbhid.a ../utility/debug/libutility.a
release:PRE_TARGETDEPS = ../hid/release/libhid.a ../usbhid/release/libusbhid.a ../utility/release/libutility.a

PRECOMPILED_HEADER = stdafx.h
DEFINES += USING_PCH ULAPI
QMAKE_CXXFLAGS += -include stdafx.h

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
include(hidcomp.pri) 

