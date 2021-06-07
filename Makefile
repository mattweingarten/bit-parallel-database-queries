



run:
	gcc -O3 -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c  main.c -o run -lm -march=native

clean:
	rm  run 

profile:
	gcc -O3 -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c  main.c -o run -lm -march=native -D _PROFILE
	
# optim: gcc -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c  main.c -o run -lm -march=native

