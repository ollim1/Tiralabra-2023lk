# Script to get CFLAGS for check depending on the distro.
# For some reason pkg-config gives the flags in the wrong order
# on Ubuntu and it doesn't work. This script prints the correct working
# flags manually

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
elif type lsb_release >/dev/null 2>&1; then
    OS=$(lsb_release -si)
elif [ -f /etc/lsb-release ]; then
    . /etc/lsb-release
    OS=$DISTRIB_ID
fi

if [ "$OS" = "Ubuntu" ]; then
    echo "-lcheck -lcheck_pic -lsubunit -pthread -lrt -lm"
else
    echo $(pkg-config --cflags --libs check)
fi
