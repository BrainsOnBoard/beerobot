#!/bin/sh
# This is a script to set the display settings so that the screen is correctly mirrored on Sussex's stupid overelaborate display setup. Might only work on Alex's laptop.

xrandr --output eDP1 --mode 1920x1080 --pos 0x0 --rotate normal --output HDMI1 --mode 1920x1080 -r 50 --pos 0x0 --rotate normal
