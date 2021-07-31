  
all:
	g++ -std=c++11 -g -pthread quicksort.cpp Mysort.cpp minheap.cpp -o ms.out