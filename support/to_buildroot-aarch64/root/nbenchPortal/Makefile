#Macros
default: objects
ACC = aarch64-none-linux-gnu-gcc
CFLAGS = -g -static -O2
DEFINES= -DLINUX $(NO_UNAME)

#Dependencies
sysinfoc.c: Makefile
	./sysinfo.sh a

sysinfo.c: Makefile
	./sysinfo.sh a
	

hardware.o: hardware.c hardware.h Makefile
	$(ACC) $(DEFINES) $(CFLAGS)\
		-c hardware.c

nbench0.o: nbench0.h nbench0.c nmglobal.h pointer.h hardware.h\
	   Makefile sysinfo.c sysinfoc.c
	$(ACC) $(DEFINES) $(CFLAGS)\
		-c nbench0.c

emfloat.o: emfloat.h emfloat.c nmglobal.h pointer.h Makefile
	$(ACC) $(DEFINES) $(CFLAGS)\
		-c emfloat.c

pointer.h: pointer Makefile
	$(ACC) $(DEFINES) $(CFLAGS)\
		-o pointer pointer.c
	rm -f pointer.h
	echo "#define LONG64" >pointer.h

misc.o: misc.h misc.c Makefile
	$(ACC) $(DEFINES) $(CFLAGS)\
		-c misc.c

nbench1.o: nbench1.h nbench1.c wordcat.h nmglobal.h pointer.h Makefile
	$(ACC) $(DEFINES) $(CFLAGS)\
		-c nbench1.c

sysspec.o: sysspec.h sysspec.c nmglobal.h pointer.h Makefile
	$(ACC) $(DEFINES) $(CFLAGS)\
		-c sysspec.c

objects: emfloat.o misc.o nbench0.o nbench1.o sysspec.o hardware.o

##########################################################################
clean:
	- /bin/rm -f *.o *~ \#* core a.out sysinfo.c sysinfoc.c \
		 bug pointer pointer.h debugbit.dat
