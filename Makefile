output: main.o
	g++ -Wall $(DEBUG) -std=c++11 main.o -o bin2csv
main.o:	main.cpp
	g++ -Wall $(DEBUG) -std=c++11 -c main.cpp

debug: DEBUG = -g

debug: main.o output
	
clean:
	rm *.o bin2csv
