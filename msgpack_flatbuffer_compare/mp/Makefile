all:
	g++ -o mp_benchmark benchmark.cpp -I./
	g++ -O1 -o mp_benchmark_O1 benchmark.cpp -I./
	g++ -O2 -o mp_benchmark_O2 benchmark.cpp -I./
	g++ -O3 -o mp_benchmark_O3 benchmark.cpp -I./

clean:
	rm -rf mp_benchmark*
