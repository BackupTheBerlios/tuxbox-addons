#! /bin/sh

cat <<END >/etc/ipkg/official-updated-ronaldd-feed.conf
src/gz official-updated-ronaldd http://ronaldd.irde.to/ipkg/official-updated-ronaldd
END

ipkg update
ipkg upgrade
