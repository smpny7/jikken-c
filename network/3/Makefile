server:
	gcc -Wall server.c -o server
	./server
	rm -rf *~ server

meibo:
	gcc -Wall meibo_server.c -o meibo
	./meibo
	rm -rf *~ meibo

client:
	gcc -Wall client.c -o client
	./client < input.txt
	rm -rf *~ client

clean:
	rm -rf *~ server meibo client

# End of file (Makefile)
