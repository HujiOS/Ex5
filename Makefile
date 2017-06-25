CC=g++
RANLIB=ranlib

LIBOBJ=CacheFS.o

CFLAGS = -Wall -Wextra -c -g
LOADLIBES = -L./
LFLAGS = -o

MORECLEAN=whatsappServer whatsappClient

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS=$(LIBSRC) Makefile README whatsappClient.cpp whatsappServer.cpp Constants.h

all: whatsappServer whatsappClient

whatsappServer : whatsappServer.o
	$(CC) $^ -o $@

whatsappClient : whatsappClient.o
	$(CC) $^ -o $@

TEST.o : TEST.cpp
	$(CC) $(CFLAGS) $^ -o $@

TEST : TEST.o
	$(CC) TEST.o $(LOADLIBES) CacheFS.a -o TEST

%.o: %.cpp
	$(CC) $(CFLAGS) $^ -o $@

#CacheFS.a: $(LIBOBJ) myFile.o Block.o
#	$(AR) $(ARFLAGS) $@ $^
#	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(OBJ) $(LIBOBJ) $(MORECLEAN) *~ *core *.o

depend:
	makedepend -- $(CFLAGS) -- $(SRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

jona:
	mv -f Search jonas/Test_496/Search; \
	mv -f libMapReduceFramework.a jonas/Framework/MapReduceFramework.a; \
	cd jonas; \
	./compile_suite; \
	./run_suite; \
	cd ..;

