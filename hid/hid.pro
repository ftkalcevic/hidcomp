TEMPLATE = subdirs
SUBDIRS = utility \
	  usbhid \
	  hid \
	  qtcommon \
	  hidconfig \
	  hidcomp

CONFIG += ordered
include(build.pri)

