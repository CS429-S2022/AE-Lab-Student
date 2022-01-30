# Definitions

CC = gcc
# CC_FLAGS = -fomit-frame-pointer -fno-asynchronous-unwind-tables -ggdb
CC_FLAGS = -ggdb
CC_OPTIONS = -c
CC_SO_OPTIONS = -shared -fpic
CC_DL_OPTIONS = -rdynamic
OD = objdump
OD_FLAGS = -d -h -r -s -S -t 
RM = /bin/rm -f
LD = gcc
LIBS = -ldl

HDRS := \
archsim.h \
elf_loader.h \
err_handler.h \
handle_args.h \
instr.h \
interface.h \
machine.h \
mem.h \
proc.h \
ptable.h \
reg.h
SRCS := $(HDRS:%.h=%.c)
OBJS := $(SRCS:%.c=%.o)

#SRCSREF := archsim.c handle_args.c interface.c err_handler.c machine.c elf_loader.c mem.c ptable.c
#OBJSREF := $(SRCS:%.c=%.o)

TESTDIR := tests
TESTS := ${TESTDIR}/test_simple.txt

# Generic rules

%.i: %.c
	${CC} ${CC_OPTIONS} -E ${CC_FLAGS} $<

%.s: %.c
	${CC} ${CC_OPTIONS} -S ${CC_FLAGS} $<

%.o: %.c
	${CC} ${CC_OPTIONS} ${CC_FLAGS} $<

# Targets

all: ae clean

ae: ${OBJS} ${HDRS}
	${CC} ${CC_FLAGS} -o $@ ${OBJS}

# ae_reference: ${SRCSREF} ${HDRS}
# 	${CC} -fomit-frame-pointer -fno-asynchronous-unwind-tables -O2 -o $@ ${SRCSREF}

# test: ci
# 	chmod +x driver.sh
# 	./driver.sh ${TESTS}

clean:
	${RM} *.o *.so
