.SILENT:

all: webserver

webserver: webserver.cc
	gcc -o webserver webserver.cc

.PHONY: clean

clean:
	rm webserver
