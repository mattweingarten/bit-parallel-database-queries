
run:
	gcc -w ./src/validate.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c  main.c -o run -lm

clean:
	rm  run 