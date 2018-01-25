#!/bin/sh
# script to start robot server

echo 'cd beerobot && nohup ./beerobot' | ssh ubuntu@tegra-ubuntu.local |& grep -v Corrupt

