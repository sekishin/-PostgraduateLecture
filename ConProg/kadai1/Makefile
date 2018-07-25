LOCK: server.c client.c
	cc -O2 -pipe -DWITH_FLOCK -o server server.c
	cc -O2 -pipe -o client client.c

UNLOCK: server.c client.c
	cc -O2 -pipe -o server server.c
	cc -O2 -pipe -o client client.c
