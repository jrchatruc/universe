default:
	mkdir -p build && g++ -std=c++11 src/main.cpp src/linalg.cpp src/universe.cpp -I/usr/local/include -L/usr/local/lib -lSDL2 -o build/main
run:
	./build/main
clean:
	rm -rf ./build