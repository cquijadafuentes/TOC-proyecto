CPP=g++

BINS=MiniBitmap test_minibitmap load_input checker_output test_minibitmap_subc

CPPFLAGS=-Wall -g -O2 -std=c++11 -O3 -DNDEBUG -w -lm

all: clean bin

bin: $(BINS)

MiniBitmap:
	$(CPP) $(CPPFLAGS) -c -o MiniBitmap.o ../MiniBitmap.cpp 

test_minibitmap:
	$(CPP) $(CPPFLAGS) -o test_minibitmap test_minibitmap.cpp MiniBitmap.o

test_minibitmap_subc:
	$(CPP) $(CPPFLAGS) -o test_minibitmap_subc test_minibitmap_subc.cpp MiniBitmap.o

load_input:
	$(CPP) $(CPPFLAGS) -o load_input load_input.cpp MiniBitmap.o

checker_output:
	$(CPP) $(CPPFLAGS) -o checker_output checker_output.cpp MiniBitmap.o

clean:
	rm -f MiniBitmap.o $(BINS)
	cd .; rm -f *.a $(BINS)