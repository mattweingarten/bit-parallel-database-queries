run:

	gcc -O0 -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c ./src/query3_archive.c ./src/tsc.c main.c -o run -lm -march=native

clean:
	rm  run 

profile:
	gcc -O0 -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c ./src/tsc.c  main.c -o run -lm -march=native -D _PROFILE
	
total:
	gcc -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c ./src/tsc.c main.c -o run -lm -march=native -D _PROFILE_TOTAL

optim: 	
	gcc -O2 -w ./src/generator.c ./src/validate.c ./src/perform.c ./src/unit_tests.c ./src/query1.c ./src/query2.c ./src/query3.c ./src/debug.c ./src/converter.c ./src/query3_archive.c  ./src/tsc.c main.c -o run -lm -march=native


sql:
	gcc -O0 -std=c99 ./src/generator.c ./src/tsc.c ./sql_benchmark.c -o sql_benchmark `mysql_config --cflags --libs`