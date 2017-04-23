# Makefile
CC       = clang
RM       = rm -rf
CFLAGS   = -Wall -Wextra -g `pkg-config sdl2 --cflags` -O3
SRCPATH  = ./src
GENERATOR_DIR ?= ../1_Image-Generator_pthread
SHRPATH  = $(GENERATOR_DIR)/shared/src
INCPATH  = -I./$(GENERATOR_DIR)/shared/include
LIBPATH  =
LIBS     = `pkg-config sdl2 --libs`
SRC      = $(SRCPATH)/image_viewer.c
SRC     += $(wildcard $(SHRPATH)/*.c)
TARGET   = image_viewer

PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	INCPATH  += -I /Library/Frameworks/SDL2.framework/Headers
	LIBS     = -framework SDL2
	CFLAGS   = -Wall -g -O3
endif

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRC) $(INCPATH) $(LIBPATH) $(LIBS)

clean:
	$(RM) $(TARGET) $(TARGET).dSYM
