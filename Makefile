SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
LD = ld

# define nullptr, but not NULL.
CFLAGS = -m32 -Dnullptr=0
ASFLAGS =
CCFLAGS = -g -std=c11 -Wall -g -fno-stack-protector -ffreestanding
LDFLAGS = -g -melf_i386 -Tkernel.ld

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(addprefix obj/, $(notdir $^))

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

%.o: %.S
	$(CC) $(CFLAGS) $(ASFLAGS) -c -o $(addprefix obj/, $(notdir $@)) $^

clean:
	rm $(addprefix obj/, $(notdir $(OBJS)))

run:
	qemu-system-i386 -kernel kernel

.PHONY: clean
.PHONY: run
