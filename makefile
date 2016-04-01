ESDK=${EPIPHANY_HOME}
ELIBS=-L ${ESDK}/tools/host/lib
EINCS=-I ${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf
EMSELF=./Debug/e_mstring.elf
HOSTCFLAGS=-std=c++11 ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread -lncurses -g
HOSTCC=arm-linux-gnueabihf-g++
DEVICECC=e-gcc
DEVICECFLAGS=-T ${ELDF} -le-lib -lm -Os
HOSTELF=Debug/host.elf
DEVICESREC=Debug/e_mstring.srec
.PHONY:all
.PHNOY:clean
all: $(HOSTELF) $(DEVICESREC)
$(HOSTELF): ./src/host.cpp
	$(HOSTCC) src/host.cpp -o $(HOSTELF) $(HOSTCFLAGS)
$(EMSELF): ./src/e_msting.c ./src/mymath.c
	$(DEVICECC) src/e_msting.c -o $(EMSELF) $(DEVICECFLAGS)
$(DEVICESREC): $(EMSELF)
	e-objcopy --srec-forceS3 --output-target srec Debug/e_mstring.elf $(DEVICESREC)
clean:
	rm Debug/*.*
