#! /bin/sh

# This script is called by setup plugin when the emu menu is activated.

>/var/tmp/emulist
NOT="gbox1"
[ -x /bin/gbox1 -o -x /var/bin/gbox1 ] && NOT=abcdefghijk

ls -r 2>/dev/null /etc/init.d/emu_* |grep -v '.bak$' |grep -v $NOT |while read x
do
  #y=`echo $x|cut -d. -f2-`
  y=`echo $x |sed -e 's/^.*emu_//'`
  echo $y
done |sort -ur >/var/tmp/emulist
