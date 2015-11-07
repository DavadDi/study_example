all:
	./flatc -c -b app.fbs
	g++ -o app -std=c++0x app.cpp -I../

clean:
	rm -rf app app_generated.h
