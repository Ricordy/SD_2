
CFLAGS = -Wall -I include -g
CC = gcc
SRC = source
INCLUDE = include
OBJ = object
BIN = binary
LIB = lib

out:
	make data.o sdmessage entry.o tree.o message.o client_stub.o network_client.o  

data.o: $(SRC)/data.c $(INCLUDE)/data.h
	$(CC) -c $(CFLAGS) $(SRC)/data.c -o $(OBJ)/data.o




entry.o: $(SRC)/entry.c $(INCLUDE)/data.h $(INCLUDE)/entry.h
	$(CC) -c $(CFLAGS) $(SRC)/entry.c -o $(OBJ)/entry.o

tree.o: $(SRC)/tree.c $(INCLUDE)/data.h $(INCLUDE)/entry.h $(INCLUDE)/tree.h $(INCLUDE)/tree-private.h
	$(CC) -c $(CFLAGS) $(SRC)/tree.c -o $(OBJ)/tree.o

tree_skel.o: $(SRC)/tree_skel.c  $(INCLUDE)/tree.h $(INCLUDE)/sdmessage.pb-c.h
	$(CC) -c $(CFLAGS) $(SRC)/tree_skel.c -o $(OBJ)/tree_skel.o

sdmessage: 
	protoc --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c $(SRC)/
	mv sdmessage.pb-c.h $(INCLUDE)/


client_stub.o: $(SRC)/client_stub.c  $(INCLUDE)/data.h $(INCLUDE)/entry.h $(INCLUDE)/entry.h $(INCLUDE)/sdmessage.pb-c.h
	$(CC) -c $(CFLAGS) $(SRC)/client_stub.c -o $(OBJ)/client_stub.o

network_client.o: $(SRC)/network_client.c $(INCLUDE)/network_client.h $(INCLUDE)/inet.h $(INCLUDE)/client_stub_private.h $(INCLUDE)/message-private.h
	$(CC) -c $(CFLAGS) $(SRC)/network_client.c -o $(OBJ)/network_client.o

network_server.o: $(SRC)/network_server.c $(INCLUDE)/network_server.h $(INCLUDE)/inet.h $(INCLUDE)/message-private.h
	$(CC) -c $(CFLAGS) $(SRC)/network_server.c -o $(OBJ)/network_server.o

message.o: $(SRC)/message.c $(INCLUDE)/message-private.h 
	$(CC) -c $(CFLAGS) $(SRC)/message.c -o $(OBJ)/message.o

clean:
	rm -f $(OBJ)/*
	rm -f $(BIN)/*
	rm -f $(LIB)/*
	rm -f $(SRC)/sdmessage.pb-c.c
	rm -f $(INCLUDE)/sdmessage.pb-c.h
