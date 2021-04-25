
run:
	gcc -w ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c  main.c -o run

clean:
	rm  run 