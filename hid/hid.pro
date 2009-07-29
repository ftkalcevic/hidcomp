TEMPLATE = subdirs
SUBDIRS = utility \
	  usbhid \
	  hid \
	  hidconfig \
	  hidcomp

CONFIG += ordered
include(build.pro)

