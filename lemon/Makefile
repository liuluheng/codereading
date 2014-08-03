CC = clang
CXX = clang++
CXXFLAGS =
LDFLAGS  = -lc++ -lc++abi

all: lemon clean

lemon: lemon.o
	$(CC) -o $@ $^ $(LDFLAGS)  

.c.o:
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f example1.cc example1.h example1.out *.o

indent:
	indent -kr -l120 -br -ce -npsl -nut -i4 *.[hc]
