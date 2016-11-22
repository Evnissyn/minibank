CC	=	gcc
CFLAGS	=	-g -pthread
COMPILE	=	$(CC) $(CFLAGS)


server:		main.c server.h interface.h bank.h
	$(COMPILE) -o server.exe main.c server.c interface.c

client:		client.c
	$(COMPILE) -o client.exe client.c

s:		main.c server.h interface.h bank.h
	gcc -g -o server.exe main.c server.c interface.c

c:		client.c
	gcc -g -o client.exe client.c


clean:
	rm client.exe server.exe
