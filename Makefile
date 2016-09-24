PROG=json
INPUT=json.c

all:
	mkdir bin
	gcc -O3 -g $(INPUT) -o bin/$(PROG)

clean:
	rm -rf bin

clean-all: clean
	rm -rf bin
