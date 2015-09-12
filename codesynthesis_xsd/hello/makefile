all:
	xsd cxx-tree --std c++11 hello.xsd
	g++ -o driver driver.cxx hello.cxx -I./ -std=c++11 -lxerces-c
