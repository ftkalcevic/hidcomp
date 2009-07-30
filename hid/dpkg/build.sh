cp ../hidcomp/release/hidcomp debian/usr/bin
cp ../hidconfig/release/hidconfig debian/usr/bin
cp src/hidcomp.9 debian/usr/share/man/man9/hidcomp.9
gzip -9 -f debian/usr/share/man/man9/hidcomp.9
cp src/hidconfig.1 debian/usr/share/man/man1/hidconfig.1
gzip -9 -f debian/usr/share/man/man1/hidconfig.1
cp src/copyright debian/usr/share/doc/hidcomp/copyright
cp src/control debian/DEBIAN/control

dpkg-deb --build debian .

