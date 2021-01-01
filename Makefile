CPP=g++

OBJETOS=MiniBitmap.o

BINS=test_minibitmap load_input generate_input

CPPFLAGS=-Wall -g -O2 -std=c++11 -O3 -DNDEBUG -w -lm

%.o: %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

all: clean bin

bin: $(OBJETOS) $(BINS)

test_minibitmap:
	$(CPP) $(CPPFLAGS) -o ./test_minibitmap test_minibitmap.cpp $(OBJETOS)

load_input:
	$(CPP) $(CPPFLAGS) -o ./load_input load_input.cpp $(OBJETOS)

generate_input:
	$(CPP) $(CPPFLAGS) -o ./generate_input generate_input.cpp $(OBJETOS)
	

clean:
	rm -f $(OBJETOS) $(BINS)
	cd .; rm -f *.a $(BINS)