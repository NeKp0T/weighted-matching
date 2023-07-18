TARGET = A.elf
TARGETW = W.elf

CFLAGS_COMMON = -Wall -Wextra -Wshadow -std=c++14 -I include
CFLAGS_DEBUG = -g -D_DEBUG -O0
CFLAGS_RELEASE = -O2
CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_RELEASE)

all: $(TARGET) $(TARGETW)

# runs a bunch of preset tests from wtests/
test: $(TARGETW) check
	./test.sh

# runs tests on every graph in order (w/ random weights)
stress_test: $(TARGETW) next_graph check next_graph
	./all_test.sh

# random tests on small graphs
random_test: $(TARGETW) check
	./random_test.sh

valg_test: $(TARGET)
	valgrind --tool=memcheck --leak-check=full $(TARGET) < schedule.in

$(TARGET): A.cpp
	g++ A.cpp -o $@ $(CFLAGS) 

$(TARGETW): A.cpp
	g++ A.cpp -o $@ $(CFLAGS) -D WEIGHT

check: check.cpp
	g++ check.cpp -o check
	
next_graph: next_graph.cpp
	g++ next_graph.cpp -o next_graph

%.elf: %.cpp
	g++ $< -o $@ $(CFLAGS)

%.h:

clean: 
	rm -rf $(TARGET) *.elf *.o rand_wgraph.in graph check
	
.PHONY: all clean show run test	all_test random_test
