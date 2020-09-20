F2S2: main.o index.o helper.o
	gcc main.o index.o helper.o -o F2S2 -lm

main.o: ./src/main.c
	gcc -c ./src/main.c -lm
index.o: ./src/index.c ./header/index.h ./header/config.h
	gcc -c ./src/index.c 
helper.o: ./src/helper.c ./src/helper.c
	gcc -c ./src/helper.c
clean:
	rm *.o 


