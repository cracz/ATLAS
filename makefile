#Compiler
CC = g++

#Compiler Flags
CFLAGS  = `root-config --cflags --libs`

TARGET = all
OBJ = dmcHist dmcMake

$(TARGET): $(OBJ)

dmcHist: dmcHist.cxx TreeConnector.h
	$(CC) -g -o dmcHist dmcHist.cxx TreeConnector.h $(CFLAGS)

dmcMake: dmcMake.cxx
	$(CC) -g -o dmcMake dmcMake.cxx $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~
