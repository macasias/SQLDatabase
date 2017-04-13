all: program1 program2 mydatabase.h
program1:
	g++ main.cpp -o main.out -g -Wall -lpqxx -lpq -std=c++11
program2:
	g++ database_create.cpp -o database_create.out -g -Wall -lpqxx -lpq -std=c++11
build:
	./database_create.out
test:
	./main.out
clean:
	rm -f main.out

