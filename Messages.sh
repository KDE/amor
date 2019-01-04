#! /bin/sh
./preparetips > tips.cpp
$XGETTEXT tips.cpp src/*.cpp src/*.h -o $podir/amor.pot
rm -f tips.cpp
