CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -pthread

all: main wcp_clt wcp_srv
main: main.o comptine_utils.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
wcp_srv: wcp_srv.o comptine_utils.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
wcp_clt: wcp_clt.o comptine_utils.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
comptine_utils.o: comptine_utils.c comptine_utils.h common.h config.h
main.o: main.c comptine_utils.h
wcp_clt.o: wcp_clt.c comptine_utils.h
wcp_srv.o: wcp_srv.c comptine_utils.h
.PHONY: clean
clean:
	rm -f *.o wcp_srv wcp_clt
