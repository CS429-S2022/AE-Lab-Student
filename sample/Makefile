#GCC = aarch64-linux-gnu-gcc
GCC = gcc --arch=aarch64
CLANG = clang -arch=aarch64
OBJDUMP = aarch64-linux-gnu-objdump

all :
	docker build -t arm .
	docker run -v $$(pwd):/work -t arm

hello : Makefile hello.c
	@echo "+-------+"
	@echo "| hello |"
	@echo "+-------+"
	$(GCC) -O3 -e start -nostdlib -nostdinc -static -o $@ $@.c
#	$(OBJDUMP) -D $@ > $@.disas
