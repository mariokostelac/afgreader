filter-contained:
	@echo "Preparing bin folder..."
	@mkdir -p bin
	@echo "Compiling..."
	@g++ -g -O2 -std=c++11 src/reader.cpp src/overlap.cpp src/main.cpp

clean:
	@rm -rf bin
