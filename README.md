
# Optimizing relational queries over a bit-parallel database layout #

Goal of this project is to improve performance of queries 1-3 on a bit-parallel database layout, namely a DB in an [MLWEAVING](https://arxiv.org/abs/1903.03404) format. Similiar [bit-parallel database layouts] ( https://15721.courses.cs.cmu.edu/spring2016/papers/li-sigmod2013.pdf)
## Team 38 ##
<ul>

<li> Dominik Häner  </li>
<li> Sebastian  Heinekamp  </li>
<li> Michael Sommer  </li>
<li> Matthew Weingarten  </li>

</ul>


## Query 1 ##
```
    SELECT * FROM R WHERE R.a < R.b; 

```

## Query 2 ##

```
    SELECT SUM(c) FROM R WHERE R.a < R.b;
```

## Query 3 ##

```
    SELECT * FROM R, S WHERE R.a % S.b = S.c;
```

## Code structure ##

<ul>

<li> Report:  38_report.pdf </li>
<li> Converter to MLWEAVING: src/converter.c </li>
<li> Query1 : src/query1.c</li>
<li> Query2 : src/query2.c</li>
<li> Query3 : src/query3.c and src/query3_archive.c</li>
<li> Validation : src/validate.c</li>
<li> Performance testing : src/perform.c</li>
<li> Runtime results and plots : runtimedata/ </li>
<li> Ideas tested : src/query3_archive.c and srsc/unit_tests.c</li>
</ul>


## Contributions ## 
<ul>


</ul>


<!-- Some ideas:
- keep relevant links to papers etc in this file
- store each new round of optimizations in the "models" folder
- every model should mention if it optimized select or aggregate or join
- keep track of models and their stats (cycles and what have you) in a google sheet
- do report on overleaf but idk made a folder anyway in case
- collect all outside code in the "include" folder
- keep track of input sizes, cache line sizes etc should all be in some header file (haven't made it yet)

Links:
TODO sheet : https://docs.google.com/document/d/1Nvd2QfZYnLiwGtSbvccPG93FJo8UFzxAvRzsqlhOtvI/edit
Google sheet for statistics: https://docs.google.com/spreadsheets/d/1GsCsubekCNPRbQb6wIyEAhq8gsVDXbsr5RNZdtho8D4/edit?usp=sharing

TODO Google doc: https://docs.google.com/document/d/1Nvd2QfZYnLiwGtSbvccPG93FJo8UFzxAvRzsqlhOtvI/edit 

Milestones page: https://medellin.inf.ethz.ch/courses/263-2300-ETH/

Project pdf: https://acl.inf.ethz.ch/teaching/fastcode/2021/project/project-ideas/relational-queries.pdf


Papers:

Bitweaving: https://15721.courses.cs.cmu.edu/spring2016/papers/li-sigmod2013.pdf

MLWeaving: https://arxiv.org/pdf/1903.03404.pdf -->

