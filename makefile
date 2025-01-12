CXX = g++
CXXFLAGS = -Wall

mytest: cache.o mytest.cpp
	$(CXX) $(CXXFLAGS) cache.o mytest.cpp -o mytest

cache.o: cache.h cache.cpp
	$(CXX) $(CXXFLAGS) -c cache.cpp

run:
	./mytest

val:
	valgrind ./mytest

clean:
	rm *.o
	rm *~
