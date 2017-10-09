#!/usr/bin/env bash



DATE_STRING=$(date +"%a %m/%d/%y, %I:%M %P")
echo $DATE_STRING

#DATE_STRING="\"${DATE_STRING}\""
TABLESAFE_PROCESS_STRING="\"Processed: \""
TABLESAFE_PROCESSED="\$3500.87"
TABLESAFE_PROCESS_STRING="\"Transactions: \""
TABLESAFE_PROCESS_STRING="\"Avg Daily: \""
TABLESAFE_BEER_POURED="8.267 Gal"

set -x

#convert -size 600x16 xc:black -pointsize 18 -font Nimbus-Sans-L-Bold \
#	-fill blue -depth 8 -stroke none -draw 'text 0,14 '"$DATE_DRAW_STRING"'' \
#	-fill blue -depth 8 -stroke none -draw 'text 220,14 '"$TABLESAFE_PROCESS_STRING"'' \
#	-fill green -depth 8 -stroke none -draw 'text 390,14 '"$TABLESAFE_MONEY_STRING"'' \
#	tickerString.ppm

#convert -size 200x16 xc:black -pointsize 20 -font Nimbus-Sans-L-Bold -fill red -depth 8 -stroke none -draw 'text 0,16 07/16/45      ' tickerString.ppm
 

# Used for testing. Ultimately though, i am using 'Draw' insted of 'label'
#convert -background black -fill green -font Nimbus-Sans-L-Bold -depth 8 -border 0x0 -size 400x20 -pointsize 18 label:"Hello World                " -trim tickerString.ppm

#convert -background black -fill green -font Nimbus-Sans-L-Bold -depth 8 -border 0x0 -size 800x20 -pointsize 18 label:"Hello World                " -trim +repage  hello.ppm
# Tune the crop at the end to get rid of pixels at the top

#Font from https://www.dafont.com/bitmap.php

#-density 18
convert -background "#000000" -antialias -border 0x0 -depth 8\
    -fill purple -antialias -font VCR-OSD-Mono -pointsize 14 label:''"${DATE_STRING}"'    '\
    -fill blue -antialias -font VCR-OSD-Mono -pointsize 60 label:'TableSafe Processed:  '\
    -fill green -antialias -font VCR-OSD-Mono -pointsize 60 label:''"${TABLESAFE_PROCESSED}"'      ' \
	pint_dark.ppm +append \
	pint_dark.ppm +append \
    pint_dark.ppm +append \
	-fill "#000000" -font VCR-OSD-Mono -pointsize 60 label:'_' \
    -fill green -depth 8 -font VCR-OSD-Mono -pointsize 14 label:''"${TABLESAFE_BEER_POURED}"' ' \
	pint_dark.ppm +append \
	pint_dark.ppm +append \
    pint_dark.ppm +append \
	-fill "#000000" -font VCR-OSD-Mono -pointsize 60 label:'...........................' \
	-depth 8 -antialias +append -resize 25% tickerString.ppm
convert tickerString.ppm -sharpen 0x3 tickerStringSharp.ppm

#convert pint.png -background "#000000" +append testBeer.ppm
if [ "$1" == "run" ];then
	sudo ${HOME}/workspace/rpi-rgb-led-matrix/examples-api-use/demo --led-rows=16 --led-chain=6 --led-gpio-mapping=adafruit-hat-pwm --led-pwm-bits=11 --led-pwm-lsb-nanoseconds=180 --led-brightness=90 --led-show-refresh -D 1 tickerString.ppm
fi
