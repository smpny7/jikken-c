server:
	gcc -Wall server.c -o server
	./server
	rm -rf *~ server

meibo:
	gcc -Wall meibo_server.c -o meibo
	./meibo
	rm -rf *~ meibo

client:
	rm -rf *~ client output.log
	gcc -Wall client.c -o client
	./client < input_1.txt >> output.log
	./client < input_2.txt >> output.log
	diff output.log output_success.log
	rm -rf *~ client

c:
	gcc -Wall client.c -o client

clean:
	rm -rf *~ server meibo client

# End of file (Makefile)
