#!/bin/sh

pid=`ps x | grep TCPServer | grep -v grep | sed -e 's/ [^0-9 ].*//g' | sed 's/^.*://g'`

echo $pid

gdb -p $pid
