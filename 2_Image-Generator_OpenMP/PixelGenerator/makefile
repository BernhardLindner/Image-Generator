# Makefile
TARGET   = ./../pixelGenerator.out
CC       = gcc
RM       = rm -rf
CFLAGS   = -Wall --pedantic -g -O3 -mavx -ffast-math
SRCPATH  = ./src
SHRPATH  = ./../shared/src
INCPATH  = -I./include -I./../shared/include
LIBPATH  =
LIBS     = -fopenmp
SRC      = $(wildcard $(SRCPATH)/*.c)
SRC     += $(wildcard $(SHRPATH)/*.c)

PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	CC = clang-omp
endif

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRC) $(INCPATH) $(LIBPATH) $(LIBS)

clean:
	$(RM) *~ *.o $(TARGET)
