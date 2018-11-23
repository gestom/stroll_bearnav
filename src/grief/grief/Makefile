CC=g++


SOURCES=$(TARGET).cpp

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=$(TARGET)

CFLAGS := -Wall -g -O0 -Wfatal-errors -mssse3 -msse2
LDFLAGS := -g `pkg-config opencv --libs` -Llib -Wl,-rpath='$$ORIGIN/lib'

# OpenCV
CFLAGS += `pkg-config opencv --cflags`

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) -g -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) 

cleanall:
	rm -f *.o 
