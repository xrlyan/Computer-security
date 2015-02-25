all: virus host open close tstWrappers seed

virus:
	gcc -o virus virus.c
host:
	gcc -o host host.c

open:
	gcc -c open.c
close:
	gcc -c close.c

tstWrappers:
	gcc -o tstWrappers tstWrappers.c open.o close.o

seed:
	@cp virus seed
	@printf '\xde\xad\xbe\xef' >> seed
	@cat host >> seed

clean:
	rm -rf virus host seed open.o close.o tstWrappers
