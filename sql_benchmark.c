#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mysql/mysql.h>
#include "include/generator.h"
#include "include/tsc_x86.h"
#include <unistd.h>
/*
    USAGE GUIDE
        $sudo apt install default-libmysqlclient-dev 
        $make sql
        $sudo ./sql_benchmark S_rows R_rows
*/


void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}


// void insert_tuple()


int main(int argc, char **argv) {

    if(argc != 3){
        printf("Expected S_rows and R_rows!\n");
        exit(1);
    }

    size_t S_rows = atoi(argv[1]);
    size_t R_rows = atoi(argv[2]);

    srand(time(NULL));
    printf("MySQL client version: %s\n", mysql_get_client_info());
    MYSQL *con = mysql_init(NULL);

    if (con == NULL){
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "root", "root_passwd",
    "benchmarking", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }


    if (mysql_query(con, "DROP TABLE IF EXISTS r")) {
      finish_with_error(con);
    }



    if (mysql_query(con, "CREATE TABLE r(a INT, b INT, c INT, d INT) ENGINE = MEMORY")) {
      finish_with_error(con);
  }


    if (mysql_query(con, "DROP TABLE IF EXISTS s")) {
      finish_with_error(con);
    }



    if (mysql_query(con, "CREATE TABLE s(a INT, b INT, c INT, d INT) ENGINE = MEMORY")) {
      finish_with_error(con);
    }


    for(size_t i = 0; i < S_rows;++i){
        // for(size_t j = 0; j < 4; ++j);
        uint32_t a = rand(); 
        uint32_t b = rand(); 
        uint32_t c = rand(); 
        uint32_t d = rand(); 
        char query_buffer[1024]; 
        snprintf(query_buffer, sizeof(query_buffer), "INSERT INTO s VALUES (%u, %u,%u,%u)", a, b,c,d);
        if (mysql_query(con, query_buffer)) {
            finish_with_error(con);
        }
    }



    for(size_t i = 0; i < R_rows;++i){
        // for(size_t j = 0; j < 4; ++j);
        uint32_t a = rand(); 
        uint32_t b = rand(); 
        uint32_t c = rand(); 
        uint32_t d = rand(); 
        char query_buffer[1024]; 
        snprintf(query_buffer, sizeof(query_buffer), "INSERT INTO r VALUES (%u, %u,%u,%u)", a, b,c,d);
        if (mysql_query(con, query_buffer)) {
            finish_with_error(con);
        }
    }

    printf("Finished writing data!\n");
    MYSQL_RES  *result;
    myInt64 start,end;


    //Warumup
    for(size_t i = 0; i < 1;++i){
       
        start = start_tsc();

        // if (mysql_query(con, "SELECT r.a, s.a FROM r,s WHERE r.a \% s.b = s.c")) {
        //     finish_with_error(con);
        // }
        
        if (mysql_query(con, "SELECT r.a, s.a FROM r,s WHERE r.a = s.a")) {
            finish_with_error(con);
        }
        
        end = stop_tsc(start);
        printf("%llu\n",end);
        result = mysql_store_result(con);
        mysql_free_result(result);
  
    }



    for(size_t i = 0; i < 10;++i){
       
        start = start_tsc();

        if (mysql_query(con, "SELECT r.a FROM r,s WHERE r.a \% s.b = s.c")) {
            finish_with_error(con);
        }
        end = stop_tsc(start);
        printf("%llu\n",end);
        result = mysql_store_result(con);
        mysql_free_result(result);
  
    }
    mysql_close(con);
    return 0;
}
