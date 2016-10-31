# Makefile
TARGET   = ./../pixelGenerator.out
CC       = clang
RM       = rm -rf
CFLAGS   = -Wall --pedantic -g
SRCPATH  = ./src
SHRPATH  = ./../shared/src
INCPATH  = -I./include -I./../shared/include
LIBPATH  =
LIBS     = -lOpenCL
SRC      = $(wildcard $(SRCPATH)/*.c)
SRC     += $(wildcard $(SHRPATH)/*.c)

PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	LIBS = -framework OpenCL
endif

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRC) $(INCPATH) $(LIBPATH) $(LIBS)

clean:
	$(RM) *~ *.o $(TARGET)
