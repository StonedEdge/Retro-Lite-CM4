OBJS=retrolite.o
BIN=retrolite

CFLAGS+=-Wall -g -O3 -I./lib/common $(shell libpng-config --cflags)
LDFLAGS+=-L/opt/vc/lib/ -lbcm_host -lm $(shell libpng-config --ldflags) -L./lib/lib -lraspidmx -lwiringPi -lSDL2

INCLUDES+=-I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I/usr/include/SDL2 

all: $(BIN)

%.o: %.c
	@rm -f $@ 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

$(BIN): $(OBJS)
	$(CC) -o $@ -Wl,--whole-archive $(OBJS) $(LDFLAGS) -pthread -Wl,--no-whole-archive -rdynamic
	chmod +x $@
clean:
	@rm -f $(OBJS)
	@rm -f $(BIN)
