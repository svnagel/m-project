CC = gcc
CFLAGS = -Wall -O
LDFLAGS = -lm -lc  #bibliotheque math suivie de bibliotheque C
EXEC = EMModel

all: $(EXEC)

EMModel: EMModel.o
  $(CC) $(LDFLAGS) -o EMModel EMModel.o
 
EMModel.o: EMModel.c
  $(CC) $(CFLAGS) -o EMModel.o -c EMModel.c
  
clean:
  rm -f *.o
