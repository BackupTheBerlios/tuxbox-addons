#!/bin/sh

usage()
{
		echo "Usage: $0 {start|stop|status|restart|reload}"
}

if [ $# -lt 1 ] ; then usage ; break ; fi
action=$1

case "$action" in

start)
	echo "Start emu daemon"
	/usr/bin/emud
	;;

stop)
	echo "Stopping emud daemon"
	killall >/dev/null 2>&1 emud
	;;

status)
	;;

restart|reload)
	$0 stop
	$0 start
	;;

*)
	usage
	;;

esac

exit 0
