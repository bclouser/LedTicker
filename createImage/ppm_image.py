#!/usr/bin/env python3


# Read in the file

with open("pint.ppm", 'rb') as ppm_file:
	for i in range(1):
		line=ppm_file.readline()
		print(line)

#pm_data = ppm_file.read()

#print("First byte " + ":".join("{:02x}".format(ord(c)) for c in ppm_data[0]))
#print("First byte " + "".join("{:02x}".format(ppm_data[0])) );
#print("2nd byte " + "".join("{:02x}".format(ppm_data[1])) );
#print("3rd byte " + "".join("{:02x}".format(ppm_data[2])) );
#print("4th byte " + "".join("{:02x}".format(ppm_data[3])) );
#print("5th byte " + "".join("{:02x}".format(ppm_data[4])) );
#print("6th byte " + "".join("{:02x}".format(ppm_data[5])) );
#print("7th byte " + "".join("{:02x}".format(ppm_data[6])) );
#print("8th byte " + "".join("{:02x}".format(ppm_data[7])) );
#print("9th byte " + "".join("{:02x}".format(ppm_data[8])) );
#print("10th byte " + "".join("{:02x}".format(ppm_data[9])) );
#print("11th byte " + "".join("{:02x}".format(ppm_data[10])) );
#print("12th byte " + "".join("{:02x}".format(ppm_data[11])) );



