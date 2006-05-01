#! /bin/sh
perl ./preparetips > tips.cpp
$XGETTEXT tips.cpp *.cpp -o $podir/amor.pot
rm -f tips.cpp
