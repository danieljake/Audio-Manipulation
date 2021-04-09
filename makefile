CPP=g++
CPPFLAGS=-std=c++11 -g
EXE_NAME=samp

$(EXE_NAME): main.cpp sound.h
	$(CPP) -o $(EXE_NAME) main.cpp $(CPPFLAGS)

test:
	$(CPP) -o testsamp test.cpp $(CPPFLAGS)
	./testsamp

clean:
	rm -f *.o $(EXE_NAME) testsamp