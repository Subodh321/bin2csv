output: main.o
	g++ -Wall -g -std=c++11 main.o -o bin2csv
main.o:	main.cpp
	g++ -Wall -g -std=c++11 -c main.cpp
clean:
	rm *.o bin2csv
