CFLAGS=-O2 -Wall #-fomit-frame-pointer

all: key_gen encrypt decrypt

clean: 
	rm -rf *.o key_gen encrypt decrypt test1024

package: clean
	tar czvf knapsack-`sed -n 's/.*VERSION "\([^"]*\)"/\1/p' < config.h`.tgz *


encrypt: encrypt.o uint1024.o ks_crypt.o
	gcc -o encrypt encrypt.o uint1024.o ks_crypt.o

decrypt: decrypt.o uint1024.o ks_crypt.o
	gcc -o decrypt decrypt.o uint1024.o ks_crypt.o

test1024: uint1024.c uint1024.h config.h
	gcc -o test1024 $(LDFLAGS) -DDEBUG1024=1 uint1024.c

key_gen: key_gen.o uint1024.o ks_crypt.o 
	gcc -o key_gen $(LDFLAGS) key_gen.o uint1024.o ks_crypt.o



encrypt.o: encrypt.c ks_crypt.h uint1024.h config.h
	gcc -o encrypt.o ${CFLAGS} -c encrypt.c

decrypt.o: decrypt.c ks_crypt.h uint1024.h config.h
	gcc -o decrypt.o ${CFLAGS} -c decrypt.c

key_gen.o: key_gen.c uint1024.h config.h ks_crypt.h
	gcc -o key_gen.o ${CFLAGS} -c key_gen.c

uint1024.o: uint1024.c uint1024.h config.h
	gcc -o uint1024.o ${CFLAGS} -c uint1024.c

ks_crypt.o: ks_crypt.h ks_crypt.c uint1024.h config.h 
	gcc -o ks_crypt.o ${CFLAGS} -c ks_crypt.c

