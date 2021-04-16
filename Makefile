.SILENT:

all: server

server: webserver.o HTTPRequestParser.o
	g++ -Wall -g -o server HTTPRequestParser.o webserver.o

webserver.o: webserver.cc
	g++ -c -Wall -g webserver.cc

HTTPRequestParser.o: HTTPRequestParser.cc
	g++ -c -Wall -g HTTPRequestParser.cc

.PHONY: clean

clean:
	rm -f server webserver.o HTTPRequestParser.o
