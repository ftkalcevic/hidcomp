dh_testdir
dh_testroot
dh_clean 
dh_installdirs

mkdir -p debian/tmp/usr/bin
mkdir -p debian/tmp/usr/share/doc/hidcomp
mkdir -p debian/tmp/usr/share/man/man1
mkdir -p debian/tmp/usr/share/man/man9

cp ../hidcomp/release/hidcomp debian/tmp/usr/bin
cp ../hidconfig/release/hidconfig debian/tmp/usr/bin
chrpath -d debian/tmp/usr/bin/hidcomp
chrpath -d debian/tmp/usr/bin/hidconfig
cp ../hidcomp/hidcomp.9 debian/tmp/usr/share/man/man9/hidcomp.9
cp ../hidconfig/hidconfig.1 debian/tmp/usr/share/man/man1/hidconfig.1
cp debian/copyright debian/tmp/usr/share/doc/hidcomp/copyright

dh_installchangelogs

dh_strip
dh_compress
dh_fixperms
dh_installdeb
dh_makeshlibs
dh_shlibdeps
dh_gencontrol
dh_md5sums
dh_builddeb


