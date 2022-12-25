OBJECTS = main.o
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	g++ -o $(EXECUTABLE) $(OBJECTS) -lpython3.8
	mkdir -p Build
	mv $(EXECUTABLE) $(OBJECTS) Build

$(OBJECTS): %.o: %.cpp
	g++ -I/usr/include/python3.8/ -c main.cpp

clean:
	rm -f -r ./Build
