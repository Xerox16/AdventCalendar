#A sample Makefile
CXX = g++
LINK_TARGET = ../opengl.exe
DEBUG = yes

ifeq ($(DEBUG),yes)
	CXXFLAGS=-I$(IDIR) -g -Wall -ansi -Wextra -O2 -Werror -std=c++11
else 
	CXXFLAGS=-I$(IDIR) -Os
endif
 
ODIR=obj
LDFLAGS= -g -Wall
LIBS= -lGL -lSDL -lSDL_mixer -lSDL_ttf -lGLEW
LDIR= -I../include -I/usr/include/SDL -I/usr/include/GLEW
IDIR= ../include

_DEPS=main.h calendarreader.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o calendarreader.o
OBJ =$(patsubst %,$(ODIR)/%,$(_OBJ))


.SUFFIXES:

all: $(LINK_TARGET)

ifeq ($(DEBUG), yes)
	@echo "DEBUG"

else 
	@echo "RELEASE"
endif
	

$(LINK_TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(LDIR)

.PHONY: clean mrproper

clean:
	rm -f $(ODIR)/*.o 

mrproper: clean
	rm -f $(LINK_TARGET)
