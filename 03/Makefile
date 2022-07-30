CC = gcc
FLGAGS = -lpthread -g
DST = main
SRC = $(DST).c

OBJS = $(DST).o 
OBJS += $(addprefix ../mir/, mir.o mir-gen.o c2mir/c2mir.o)

%.o: %.c
	$(CC) -I ../mir -c -o $@ $<

build: $(OBJS)
	-$(CC) $(FLGAGS) -o $(DST) $^

run: build
	./$(DST)

clean:
	-$(RM) $(DST) *.o *.mirb