CPP=g++

OBJETOS=MiniBitmap.o SRP_Utils.o SRP_InstanceOutput.o SRP_InstanceInput.o

BINS=test_minibitmap test_minibitmap_subc generate_input\
		SRPlanner_test SRPlanner_checker \
		SRPlanner_solucionar

SRP=SRPlanner

CPPFLAGS=-Wall -g -O2 -std=c++11 -O3 -DNDEBUG -w -lm

ORTOOLSOBJ=-fPIC -std=c++17 -O4 -DNDEBUG -Iinclude -I. -DARCH_K8 -Wno-deprecated \
		-DUSE_BOP -DUSE_GLOP -DUSE_CBC -DUSE_CLP -DUSE_SCIP

ORTOOLSFLAGS=-fPIC -std=c++17 -O4 -DNDEBUG -Iinclude -I. -DARCH_K8 -Wno-deprecated \
		-DUSE_BOP -DUSE_GLOP -DUSE_CBC -DUSE_CLP -DUSE_SCIP -Llib -Llib64 -lortools \
		-Wl,-rpath,"./lib" -lz -lrt -lpthread 

%.o: %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

all: clean bin

bin: $(OBJETOS) $(SRP) $(BINS)

SRPlanner:
	$(CPP) $(ORTOOLSOBJ) -c -o SRPlanner.o SRPlanner.cpp 

generate_input:
	$(CPP) $(CPPFLAGS) -o ./generate_input generate_input.cpp $(OBJETOS)

test_minibitmap:
	$(CPP) $(CPPFLAGS) -o test_minibitmap old/test_minibitmap.cpp $(OBJETOS)

test_minibitmap_subc:
	$(CPP) $(CPPFLAGS) -o test_minibitmap old/test_minibitmap.cpp $(OBJETOS)
	
SRPlanner_test:
	$(CPP) $(ORTOOLSFLAGS) -o ./SRPlanner_test SRPlanner_test.cpp $(OBJETOS) SRPlanner.o
	
SRPlanner_checker:
	$(CPP) $(ORTOOLSFLAGS) -o ./SRPlanner_checker SRPlanner_checker.cpp $(OBJETOS) SRPlanner.o

SRPlanner_mincostf:
	$(CPP) $(ORTOOLSFLAGS) -o ./SRPlanner_mincostf SRPlanner_mincostf.cpp $(OBJETOS) SRPlanner.o

SRPlanner_solucionar:
	$(CPP) $(ORTOOLSFLAGS) -o ./SRPlanner_solucionar SRPlanner_solucionar.cpp $(OBJETOS) SRPlanner.o

clean:
	rm -f $(OBJETOS) $(BINS)
	cd .; rm -f *.a $(BINS)