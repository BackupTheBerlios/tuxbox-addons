#! /bin/sh

. /etc/rc.config

ls /etc/init.d/emu_* /etc/init.d/dccamd |while read x
do
  $x stop
done

sleep 1

/etc/rcS.d/S40emu start
