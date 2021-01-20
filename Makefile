CPP=g++

OBJETOS=MiniBitmap.o SRPlanner.o

BINS=test_minibitmap test_minibitmap_subc load_input checker_output \
		generate_input  SRPlanner_test SRPlanner_checker \
		SRPlanner_mincostf

CPPFLAGS=-Wall -g -O2 -std=c++11 -O3 -DNDEBUG -w -lm

%.o: %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

all: clean bin

bin: $(OBJETOS) $(BINS)

test_minibitmap:
	$(CPP) $(CPPFLAGS) -o ./old/test_minibitmap old/test_minibitmap.cpp $(OBJETOS)

test_minibitmap_subc:
	$(CPP) $(CPPFLAGS) -o ./old/test_minibitmap_subc old/test_minibitmap_subc.cpp $(OBJETOS)

load_input:
	$(CPP) $(CPPFLAGS) -o ./old/load_input old/load_input.cpp $(OBJETOS)

checker_output:
	$(CPP) $(CPPFLAGS) -o ./old/checker_output old/checker_output.cpp $(OBJETOS)

generate_input:
	$(CPP) $(CPPFLAGS) -o ./generate_input generate_input.cpp $(OBJETOS)
	
SRPlanner_test:
	$(CPP) $(CPPFLAGS) -o ./SRPlanner_test SRPlanner_test.cpp $(OBJETOS)
	
SRPlanner_checker:
	$(CPP) $(CPPFLAGS) -o ./SRPlanner_checker SRPlanner_checker.cpp $(OBJETOS)

SRPlanner_mincostf:
	$(CPP) $(CPPFLAGS) -o ./SRPlanner_mincostf SRPlanner_mincostf.cpp $(OBJETOS)

clean:
	rm -f $(OBJETOS) $(BINS) old/test_minibitmap old/test_minibitmap_subc old/load_input old/checker_output
	cd .; rm -f *.a $(BINS)