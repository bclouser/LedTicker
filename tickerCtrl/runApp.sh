#!/usr/bin/env bash

#Run this on a dedicated cpu. Requires isolcpu=3 at the end of /boot/cmdline.txt
sudo taskset 03 ./runText --led-rows=16 --led-chain=6 --led-gpio-mapping=adafruit-hat-pwm --led-pwm-bits=11 --led-pwm-lsb-nanoseconds=200 --led-brightness=90 --led-daemon not_even_using_this_param.ppm

#sudo ./runText --led-rows=16 --led-chain=6 --led-gpio-mapping=adafruit-hat-pwm --led-pwm-bits=2 --led-pwm-lsb-nanoseconds=180 --led-brightness=90 runtext16.ppm
echo "Launched with error: $?"