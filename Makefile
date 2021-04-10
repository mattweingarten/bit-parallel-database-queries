
run:
	gcc -w ./src/debug.c ./src/converter.c  main.c -o run

clean:
	rm  run 