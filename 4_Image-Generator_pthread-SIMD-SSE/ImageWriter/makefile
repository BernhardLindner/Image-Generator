# Makefile
TARGET   = ./../imageWriter.out
CC       = clang
RM       = rm -rf
CFLAGS   = -Wall --pedantic -g
SRCPATH  = ./src
SHRPATH  = ./../shared/src
INCPATH  = -I./include -I./../shared/include
LIBPATH  =
LIBS     = 
SRC      = $(wildcard $(SRCPATH)/*.c)
SRC     += $(wildcard $(SHRPATH)/*.c)
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRC) $(INCPATH) $(LIBPATH) $(LIBS)

clean:
	$(RM) *~ *.o $(TARGET)

