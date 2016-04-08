ESDK=$(EPIPHANY_HOME)
ELIBS=-L $(ESDK)/tools/host/lib
EINCS=-I $(ESDK)/tools/host/include
ELDF=$(ESDK)/bsps/current/fast.ldf
SRCDIR=src
INCDIR=$(SRCDIR)/inc
BUILDDIR=Debug
EMSELF=$(BUILDDIR)/e_mstring.elf
HOSTCFLAGS=-std=c++11 $(EINCS) $(ELIBS) -le-hal -le-loader -lpthread -lncurses -g
HOSTCC=arm-linux-gnueabihf-g++
DEVICECC=e-gcc
DEVICECFLAGS=$(EINCS) -T $(ELDF) -le-lib -lm -Os
HOSTELF=$(BUILDDIR)/host.elf
DEVICESREC=$(BUILDDIR)/e_mstring.srec

.PHONY:all
.PHNOY:clean
all: $(HOSTELF) $(DEVICESREC)
$(HOSTELF): $(BUILDDIR)/host.o $(BUILDDIR)/tcpclient.o $(BUILDDIR)/mstring.o $(BUILDDIR)/threadForce.o $(BUILDDIR)/threadMeasurement.o
	$(HOSTCC) -o $(HOSTELF) $(BUILDDIR)/host.o $(BUILDDIR)/tcpclient.o $(BUILDDIR)/mstring.o $(BUILDDIR)/threadForce.o $(BUILDDIR)/threadMeasurement.o $(HOSTCFLAGS)
$(EMSELF): $(BUILDDIR)/modes_gen.o $(BUILDDIR)/e_mstring.o
	$(DEVICECC) -o $(EMSELF) $(BUILDDIR)/modes_gen.o $(BUILDDIR)/e_mstring.o $(DEVICECFLAGS)
$(DEVICESREC): $(EMSELF)
	e-objcopy --srec-forceS3 --output-target srec $(BUILDDIR)/e_mstring.elf $(DEVICESREC)
$(BUILDDIR)/threadForce.o: $(SRCDIR)/threadForce.cpp $(INCDIR)/threadForce.h $(INCDIR)/mstring.h $(INCDIR)/ms-data-host.h $(INCDIR)/ms-spara.h $(INCDIR)/ms-const.h
	$(HOSTCC) -c $(SRCDIR)/threadForce.cpp -o $(BUILDDIR)/threadForce.o $(HOSTCFLAGS)
$(BUILDDIR)/threadMeasurement.o: $(SRCDIR)/threadMeasurement.cpp $(INCDIR)/threadMeasurement.h $(INCDIR)/mstring.h $(INCDIR)/ms-data-host.h $(INCDIR)/ms-spara.h $(INCDIR)/ms-const.h
	$(HOSTCC) -c $(SRCDIR)/threadMeasurement.cpp -o $(BUILDDIR)/threadMeasurement.o $(HOSTCFLAGS)
$(BUILDDIR)/tcpclient.o: $(SRCDIR)/tcpclient.cpp $(INCDIR)/ms-data-host.h $(INCDIR)/ms-spara.h $(INCDIR)/ms-const.h
	$(HOSTCC) -c $(SRCDIR)/tcpclient.cpp -o $(BUILDDIR)/tcpclient.o $(HOSTCFLAGS)
$(BUILDDIR)/host.o: $(SRCDIR)/host.cpp $(INCDIR)/inp_guitar.h $(INCDIR)/host.h $(INCDIR)/ms-data-host.h $(INCDIR)/threadForce.h $(INCDIR)/threadMeasurement.h $(INCDIR)/tcpclient.h
	$(HOSTCC) -c $(SRCDIR)/host.cpp -o $(BUILDDIR)/host.o $(HOSTCFLAGS)
$(BUILDDIR)/modes_gen.o: $(INCDIR)/ms-spara.h $(INCDIR)/ms-data-device.h $(INCDIR)/modes_gen.h $(INCDIR)/ms-const.h
	$(DEVICECC) -c $(SRCDIR)/modes_gen.c -o $(BUILDDIR)/modes_gen.o $(DEVICECFLAGS)
$(BUILDDIR)/mstring.o: $(SRCDIR)/mstring.cpp $(INCDIR)/ms-data-host.h $(INCDIR)/mstring.h
	$(HOSTCC) -c $(SRCDIR)/mstring.cpp -o $(BUILDDIR)/mstring.o $(HOSTCFLAGS)
$(BUILDDIR)/e_mstring.o: $(SRCDIR)/e_mstring.c $(INCDIR)/e_mstring.h $(INCDIR)/ms-data-device.h
	$(DEVICECC) -c $(SRCDIR)/e_mstring.c -o $(BUILDDIR)/e_mstring.o $(DEVICECFLAGS)
clean:
	rm -f $(BUILDDIR)/*.*
