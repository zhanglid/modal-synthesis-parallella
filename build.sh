#!/bin/bash
rm -rf Debug
mkdir Debug
set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

CROSS_PREFIX=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac
# Build HOST side application
${CROSS_PREFIX}g++ src/host.cpp -o Debug/host.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread -lncurses -g

# Build DEVICE side program
e-gcc -T ${ELDF} src/e_msting.c -o Debug/e_mstring.elf -le-lib -lm -Os

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec Debug/e_mstring.elf Debug/e_mstring.srec

