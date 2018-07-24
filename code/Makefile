patcher: patcher.c helper.c
	gcc -std=c11 -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lcrypto patcher.c helper.c -o patcher

keygen: keygen.cpp helper.c
	g++ -std=c++11 -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lcrypto keygen.cpp helper.c -o keygen


