memFS.o: interpreter.h
memFS: memFS.o
	g++ -o memFS memFS.cpp
