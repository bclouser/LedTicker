#!/usr/bin/env python

import PIL
from PIL import Image
import os
from os.path import isfile, join
import sys, getopt

def main(argv):
	try:
		opts, args = getopt.getopt(argv, "hi:o:",["ifile=","ofile="])
	except getopt.GetoptError:
		print 'generate.py -i <rawEmojiDir> -o <outputDir>'
		sys.exit(2)

	inputDir = ''
	outputDir = ''

	for opt, arg in opts:
		if opt == '-h':
			print 'generate.py -i <rawEmojiDir> -o <outputDir>'
			sys.exit()
		elif opt in ("-i", "--ifile"):
			inputDir = arg
		elif opt in ("-o", "--ofile"):
			outputDir = arg

	if inputDir == '' or outputDir == '':
		print 'generate.py -i <rawEmojiDir> -o <outputDir>'
		sys.exit(2)


	print 'Input file is "', inputDir
	print 'Output file is "', outputDir

	if not os.path.exists(inputDir):
		print "Bad input directory"
		sys.exit()
	if not os.path.exists(outputDir):
		print "Directory doesn't exist, creating " + outputDir
		os.makedirs(outputDir)



	emojiDir=inputDir

	origEmojies = [ f for f in os.listdir(emojiDir) if isfile(join(emojiDir, f)) and (f[-3:] == 'png')]

	for raw in origEmojies:
		print "converting " + raw
		img = Image.open(join(emojiDir, raw))
		# all the emojis should be 16 x 16
		img = img.resize((16, 16), PIL.Image.ANTIALIAS)

		# Convert background transparent pixels to black
		img = img.convert("RGBA")
		pixdata = img.load()
		width, height = img.size
		for y in xrange(height):
		    for x in xrange(width):
		    	# Any pixel whose opacity is less than 200, make black
		        if pixdata[x, y][3] < 10:
		        	pixdata[x, y] = (0, 0, 0, 255)

		img.save(join(outputDir,""+raw[:-3]+"ppm"),'ppm')



if __name__ == "__main__":
	main(sys.argv[1:])