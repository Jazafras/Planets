planet: planet.o
	gcc -g -o planet planet.o -lplot -lXaw -lXmu -lXt -lXext -lSM -lICE -lX11 -lpng -lz -lm

planet.o: planet.c
	gcc -g -Wall -pedantic -std=c99 -c planet.c

