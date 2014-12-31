CXX=gcc
CXX_FLAGS=-c -Wall
CXX_LFLAGS=-ldl -lm
SOURCES=sorting_tests.c list.c stack.c argParser.c
OBJECTS=$(SOURCES:.c=.o)

EXEC=sorting_tests

all: $(SOURCES) $(EXEC)

clean:
	rm $(OBJECTS)
	rm $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CXX_LFLAGS)

%.o: %.c
	$(CXX) $(CXX_FLAGS) -o $@ $<
