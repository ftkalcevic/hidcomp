rm -rf debian
mkdir debian
mkdir debian/DEBIAN
mkdir debian/usr
mkdir debian/usr/bin
mkdir debian/usr/share
mkdir debian/usr/share/doc
mkdir debian/usr/share/doc/hidcomp
mkdir debian/usr/share/man
mkdir debian/usr/share/man/man1
mkdir debian/usr/share/man/man9
#ln -s DEBIAN/control debian/control

cp ../hidcomp/release/hidcomp debian/usr/bin
cp ../hidconfig/release/hidconfig debian/usr/bin
#dpkg-shlibdeps --ignore-missing-info  debian/usr/bin/hidconfig debian/usr/bin/hidcomp

strip debian/usr/bin/hidcomp
strip debian/usr/bin/hidconfig

cp src/hidcomp.9 debian/usr/share/man/man9/hidcomp.9
gzip -9 -f debian/usr/share/man/man9/hidcomp.9

cp src/hidconfig.1 debian/usr/share/man/man1/hidconfig.1
gzip -9 -f debian/usr/share/man/man1/hidconfig.1

cp src/copyright debian/usr/share/doc/hidcomp/copyright
cp src/changelog debian/usr/share/doc/hidcomp/changelog
gzip -9 debian/usr/share/doc/hidcomp/changelog

cp src/control debian/DEBIAN/control

fakeroot dpkg-deb --build debian .

