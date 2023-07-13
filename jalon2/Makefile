all:client	server
.PHONY:all
deps =	msg_struct.h
client:
	gcc	-Wall	-g	-o	client	client.c	$(deps)
server:
	gcc	-Wall	-g	-o	server	server.c	$(deps)
clean:
	rm	client	server