all: RUN

RUN: main.c
	gcc main.c -o main -lpthread

DEBUG: main.c
	gcc main.c -o main -DDEBUG -g3 -lpthread
