#! /bin/sh

# This script is called by setup plugin when the emu menu is activated.

ls -r 2>/dev/null /etc/init.d/emu_* |grep -v '.bak$' |while read x
do
  #y=`echo $x|cut -d. -f2-`
  y=`echo $x |sed -e 's/^.*emu_//'`
  echo $y
done |sort -ur >/var/tmp/emulist

echo auto >/var/tmp/crdsrvlist
echo none >>/var/tmp/crdsrvlist
ls -r 2>/dev/null /etc/init.d/crdsrv_* |grep -v '.bak$' |while read x
do
  echo >&2 "x=$x"
  #y=`echo $x|cut -d. -f2-`
  y=`echo $x |sed -e 's/^.*crdsrv_//'`
  echo $y
done |sort -ur >>/var/tmp/crdsrvlist
