#!/bin/sh
echo $1
if [ $1 = 'start' ]; then
    echo 'Starting'
    start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket -- -d
elif [ $1 = 'stop' ]; then
    echo 'Stopping'
    start-stop-daemon -K -n aesdsocket
fi