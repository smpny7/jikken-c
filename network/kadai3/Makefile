server:
	rm -rf *~ server
	gcc -Wall server.c -o server
	./server
	rm -rf *~ server

meibo:
	rm -rf *~ meibo
	gcc -Wall meibo_server.c -o meibo
	./meibo
	rm -rf *~ meibo

client:
	rm -rf *~ client output.log output.csv
	gcc -Wall client.c -o client
	./client < input_1.txt >> output.log
	./client < input_2.txt >> output.log
	diff output.log output_success.log
	diff output.csv output_success.csv
	rm -rf *~ client

c:
	gcc -Wall client.c -o client

clean:
	rm -rf *~ server meibo client output.log output.csv

# End of file (Makefile)
