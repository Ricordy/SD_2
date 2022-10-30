
CFLAGS = -Wall -I include -g
CC = gcc
SRC = source
INCLUDE = include
OBJ = object
BIN = binary
LIB = lib

out:
	make data.o entry.o list.o table.o serialization.o sdmessage sdmessage.o message.o client_stub.o network_client.o network_server.o table_skel.o table_client.o table_server.o client-lib.o table_client table_server


data.o: $(SRC)/data.c $(INCLUDE)/data.h
	$(CC) -c $(CFLAGS) $(SRC)/data.c -o $(OBJ)/data.o

entry.o: $(SRC)/entry.c $(INCLUDE)/data.h $(INCLUDE)/entry.h
	$(CC) -c $(CFLAGS) $(SRC)/entry.c -o $(OBJ)/entry.o

list.o: $(SRC)/list.c $(INCLUDE)/data.h $(INCLUDE)/entry.h $(INCLUDE)/list-private.h
	$(CC) -c $(CFLAGS) $(SRC)/list.c -o $(OBJ)/list.o

table.o: $(SRC)/table.c $(INCLUDE)/data.h $(INCLUDE)/entry.h $(INCLUDE)/table.h
	$(CC) -c $(CFLAGS) $(SRC)/table.c -o $(OBJ)/table.o

serialization.o: $(SRC)/serialization.c $(INCLUDE)/data.h $(INCLUDE)/entry.h $(INCLUDE)/table.h $(INCLUDE)/serialization.h
	$(CC) -c $(CFLAGS) $(SRC)/serialization.c -o $(OBJ)/serialization.o

sdmessage:
	protoc --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c $(SRC)/
	mv sdmessage.pb-c.h $(INCLUDE)/

sdmessage.o: $(SRC)/sdmessage.pb-c.c $(INCLUDE)/sdmessage.pb-c.h
	$(CC) -c $(CFLAGS) $(SRC)/sdmessage.pb-c.c -o $(OBJ)/sdmessage.pb-c.o

message.o: $(SRC)/message.c $(INCLUDE)/message-private.h $(INCLUDE)/inet.h 
	$(CC) -c $(CFLAGS) $(SRC)/message.c -o $(OBJ)/message.o

client_stub.o: $(SRC)/client_stub.c $(INCLUDE)/client_stub_private.h $(INCLUDE)/network_client.h $(INCLUDE)/inet.h $(INCLUDE)/message-private.h  $(INCLUDE)/sdmessage.pb-c.h 
	$(CC) -c $(CFLAGS) $(SRC)/client_stub.c -o $(OBJ)/client_stub.o

network_client.o: $(SRC)/network_client.c $(INCLUDE)/network_client.h $(INCLUDE)/inet.h $(INCLUDE)/client_stub_private.h $(INCLUDE)/message-private.h
	$(CC) -c $(CFLAGS) $(SRC)/network_client.c -o $(OBJ)/network_client.o

network_server.o: $(SRC)/network_server.c $(INCLUDE)/network_server.h $(INCLUDE)/inet.h $(INCLUDE)/message-private.h
	$(CC) -c $(CFLAGS) $(SRC)/network_server.c -o $(OBJ)/network_server.o

table_skel.o: $(SRC)/table_skel.c $(INCLUDE)/table_skel.h $(INCLUDE)/inet.h
	$(CC) -c $(CFLAGS) $(SRC)/table_skel.c -o $(OBJ)/table_skel.o

table_client.o: $(SRC)/table_client.c $(INCLUDE)/network_client.h $(INCLUDE)/client_stub_private.h $(INCLUDE)/inet.h
	$(CC) -c $(CFLAGS) $(SRC)/table_client.c -o $(OBJ)/table_client.o

table_server.o: $(SRC)/table_server.c $(INCLUDE)/inet.h $(INCLUDE)/table_skel.h $(INCLUDE)/network_server.h
	$(CC) -c $(CFLAGS) $(SRC)/table_server.c -o $(OBJ)/table_server.o

client-lib.o:
	ld -r -I/usr/local/include $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/client_stub.o $(OBJ)/network_client.o $(OBJ)/message.o $(OBJ)/sdmessage.pb-c.o -o $(LIB)/client-lib.o

table_client: $(OBJ)/table_client.o $(LIB)/client-lib.o
	$(CC) $(CFLAGS) -I/usr/local/include $(OBJ)/table_client.o $(LIB)/client-lib.o -o $(BIN)/table-client -L/usr/local/lib/ -lprotobuf-c

table_server: $(OBJ)/table_server.o $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/table_skel.o $(OBJ)/network_server.o $(OBJ)/message.o
	$(CC) $(CFLAGS) -I/usr/local/include $(OBJ)/table_server.o $(OBJ)/sdmessage.pb-c.o $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/table_skel.o $(OBJ)/network_server.o $(OBJ)/message.o -o $(BIN)/table-server -L/usr/local/lib/ -lprotobuf-c

clean:
	rm -f $(OBJ)/*
	rm -f $(BIN)/*
	rm -f $(LIB)/*
	rm -f $(SRC)/sdmessage.pb-c.c
	rm -f $(INCLUDE)/sdmessage.pb-c.h
