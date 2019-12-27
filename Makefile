build:
	mkdir build && cd build && cmake .. && make Codech

bench: build
	@./bench.sh -f 64 -s
	@./bench.sh -f 512 -s
	@./bench.sh -f 1024
