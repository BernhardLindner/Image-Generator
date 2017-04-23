# Makefile
TARGET   = ./../pixelGenerator.out
CC       = clang
RM       = rm -rf
CFLAGS   = -Wall --pedantic -g -O3 -msse3 -ffast-math
SRCPATH  = ./src
SHRPATH  = ./../shared/src
INCPATH  = -I./include -I./../shared/include
LIBPATH  =
LIBS     = -lpthread
SRC      = $(wildcard $(SRCPATH)/*.c)
SRC     += $(wildcard $(SHRPATH)/*.c)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRC) $(INCPATH) $(LIBPATH) $(LIBS)

clean:
	$(RM) $(TARGET) $(TARGET).dSYM
