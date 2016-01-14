all:
	g++ -o fb_benchmark -std=c++11 test_main.cpp -I./
	g++ -O1 -o fb_benchmark_O1 -std=c++11 test_main.cpp -I./
	g++ -O2 -o fb_benchmark_O2 -std=c++11 test_main.cpp -I./
	g++ -O3 -o fb_benchmark_O3 -std=c++11 test_main.cpp -I./

clean:
	rm -rf fb_benchmark*
