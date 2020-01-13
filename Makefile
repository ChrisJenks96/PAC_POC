TARGET = pac_poc
OBJS   = main.o util.o anim.o main_menu.o callbacks.o inventory.o events.o video.o

INCDIR   =
CFLAGS   = -O0 -G0 -Wall -g -D__PSP__
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)

LIBDIR  =
LDFLAGS =
LIBS    = -lSDL -lSDL_TTF -lSDL_mixer -lmikmod -lsmpeg -lpmp -lvorbisidec -logg -lmmio -lpspirkeyb -lpspwlan -lpsppower -lGL -lfreetype -ljpeg -lpng -lz -lm -lSDL -lpspgu -lpsphprm -lpspaudiocodec -lpspmpeg -lpspgu -lpspaudio -lstdc++ -lpspvfpu -lpsprtc

BUILD_PRX = 1 

EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = PAC_POC
#PSP_EBOOT_ICON= ICON0.png
#PSP_EBOOT_PIC1= PIC1.png
#PSP_EBOOT_SND0= SND0.at3

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak