CXX = g++

all: logger.so logger

logger: logger.so hw2.cpp
	$(CXX) hw2.cpp -o logger

logger.so: logger.cpp
	$(CXX) -o logger.so -shared -fPIC logger.cpp -ldl

.PHONY: clean
clean:
	rm logger.so logger