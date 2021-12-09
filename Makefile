MAPS = ./lib/maps
CC = cc

all:
	$(MAPS) -e test.s

clean:
	rm -rf *~ test.s.mem test

# End of file (Makefile)
