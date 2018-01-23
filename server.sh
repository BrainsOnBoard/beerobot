#!/bin/sh
# script to start robot server

echo 'cd beerobot && nohup ./beerobot' | ssh ubuntu@192.168.1.3 |& grep -v Corrupt

