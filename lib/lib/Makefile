#
# Build static and shared libraries for raspidmx
#

LIB=raspidmx

OBJS=../common/backgroundLayer.o ../common/imageGraphics.o ../common/key.o ../common/spriteLayer.o \
 ../common/font.o ../common/imageKey.o ../common/loadpng.o \
 ../common/hsv2rgb.o ../common/imageLayer.o ../common/savepng.o \
 ../common/image.o ../common/imagePalette.o ../common/scrollingLayer.o

CFLAGS+=-Wall -g -O3 -I../common $(shell libpng-config --cflags)
LDFLAGS+=-L/opt/vc/lib/ -lbcm_host -lm $(shell libpng-config --ldflags)

INCLUDES+=-I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

all: $(LIB)

%.o: %.c
	@rm -f $@ 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

$(LIB): $(OBJS)
	$(AR) rcs lib$(LIB).a $(OBJS)
	#$(CC) -shared -Wl,-soname,lib$(LIB).so.1 $(LDFLAGS) -o lib$(LIB).so.1 $(OBJS)

# TODO: move to debian package
	ln -sfv lib$(LIB).so.1 lib$(LIB).so

clean:
	@rm -f $(OBJS)
	@rm -f lib$(LIB).so*
	@rm -f lib$(LIB).a
