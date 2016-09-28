PROG=raycast
INPUT=json.c raycast.c ppmrw.c

all:
	if [ ! -e bin ]; then mkdir bin; fi
	gcc -O3 -g $(INPUT) -o bin/$(PROG)

clean:
	rm -rf bin

clean-all: clean
	rm -rf bin
