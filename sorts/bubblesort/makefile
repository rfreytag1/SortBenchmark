CXX=gcc
CXX_FLAGS=-c -Wall -Wextra -fPIC
CXX_LFLAGS=-shared
SOURCES=bubblesort.c ../helpers.c
OBJECTS=$(SOURCES:.c=.o)

LIB=libbubblesort

all: $(SOURCES) $(LIB)

clean:
	@rm -f $(OBJECTS)
	@rm -f $(LIB).so.1.0
	@rm -f ../../$(LIB).so.1.0

$(LIB): $(OBJECTS)
	@$(CXX) -Wl,-soname,$(LIB).so.1 -o $@.so.1.0 $(OBJECTS) $(CXX_LFLAGS)
	@cp -f $@.so.1.0 ../../$@.so.1.0

%.o: %.c
	@$(CXX) $(CXX_FLAGS) -o $@ $<

