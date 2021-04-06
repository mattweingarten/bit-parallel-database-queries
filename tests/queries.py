from numpy import np



#q1
# Level 1. (Warm-up) Let R be a relation stored in the MLWeaving layout, optimize the
# following SELECT query:
# SELECT * FROM R WHERE R.a < R.b;

# and AGGREGATE query:
# SELECT SUM(c) FROM R WHERE R.a < R.b;
# (You don't need to write the parser, your program can hard code `a, b, c`)
# Level 2. (Harder) Let R and S be two relations stored in the MLWeaving layout, optimize a
# join query such as
# SELECT * FROM R, S WHERE R.a % S.b = S.c;
# (implement the nested-loop join algorithm, your program can hard code `a, b, c`)