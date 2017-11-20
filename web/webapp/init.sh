#!/usr/bin/env bash


# Just take care of all the things that need to be done on startup

# Create database

#Add users to that database
# use LedTicker
# db.createUser({user:"someUser", pwd:"somePassword", roles:["readWrite"]})

# db.createCollection("tickerData")


mkdir certs
cd certs

echo "==== Creating signing key"
openssl genrsa -out server.enc.key 2048 || {
	echo "Failed to generate signing key"
	exit -1
}

echo "==== Creating server request"
openssl req -new -key server.enc.key -out server.csr || {
	echo "Failed to create server request \"csr\""
	exit -1
}

echo "==== Removing encryption from the private key"
openssl rsa -in server.enc.key -out server.key || {
	echo "Failed to strip off encryption of private key"
	exit -1
}

echo "==== Self signing the certficate"
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt || {
	echo "Failed to self-sign the certificate"
	exit -1
}

cd ..