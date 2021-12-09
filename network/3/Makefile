server:
	gcc -Wall server.c -o server
	./server
	rm -rf *~ server

client:
	gcc -Wall client.c -o client
	./client %C hello
	rm -rf *~ client

clean:
	rm -rf *~ server client

# End of file (Makefile)
