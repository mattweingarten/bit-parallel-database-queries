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

    if(argc != 2){
        printf("Expected Rows\n");
        exit(1);
    }

    size_t R_rows = atoi(argv[1]);

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
	
		for(size_t i = 0; i < R_rows;++i){
        // for(size_t j = 0; j < 4; ++j);
        uint32_t a = rand(); 
        uint32_t b = rand(); 
        uint32_t c = rand(); 
        uint32_t d = rand(); 
        char query_buffer[1024]; 
        snprintf(query_buffer, sizeof(query_buffer), "INSERT INTO r VALUES (%u,%u,%u,%u)", a, b,c,d);
        if (mysql_query(con, query_buffer)) {
            finish_with_error(con);
        }
    }
	
	/*
	if (mysql_query(con, "CREATE TABLE r(a INT, b INT, c INT, d INT, e INT, f INT, g INT, h INT, i INT, j INT, k INT, l INT, m INT, n INT, o INT, p INT, q INT, r INT, s INT, t INT, u INT, v INT, w INT, x INT, y INT, z INT, aa INT, ab INT, ac INT, ad INT, ae INT, af INT) ENGINE = MEMORY")) {
      finish_with_error(con);
	}
	
	for(size_t i = 0; i < R_rows;++i){
        uint32_t a = rand(); 
        uint32_t b = rand(); 
        uint32_t c = rand(); 
        uint32_t d = rand(); 
		uint32_t e = rand(); 
		uint32_t f = rand(); 
		uint32_t g = rand(); 
		uint32_t h = rand(); 
		uint32_t i = rand(); 
		uint32_t j = rand(); 
		uint32_t k = rand(); 
		uint32_t l = rand(); 
		uint32_t m = rand(); 
		uint32_t n = rand(); 
		uint32_t o = rand(); 
		uint32_t p = rand(); 
		uint32_t q = rand(); 
		uint32_t r = rand(); 
		uint32_t s = rand(); 
		uint32_t t = rand(); 
		uint32_t u = rand(); 
		uint32_t v = rand(); 
		uint32_t w = rand(); 
		uint32_t x = rand(); 
		uint32_t y = rand(); 
		uint32_t z = rand(); 
		uint32_t aa = rand(); 
		uint32_t ab = rand(); 
		uint32_t ac = rand(); 
		uint32_t ad = rand(); 
		uint32_t ae = rand(); 
		uint32_t af = rand(); 
        char query_buffer[4096]; 
        snprintf(query_buffer, sizeof(query_buffer), "INSERT INTO r VALUES (%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)", a, b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af);
        if (mysql_query(con, query_buffer)) {
            finish_with_error(con);
        }
    }
	*/



    

    printf("Finished writing data!\n");
    MYSQL_RES  *result;
    myInt64 start,end;


    //Warumup
    for(size_t i = 0; i < 10;++i){
       
        start = start_tsc();

        // if (mysql_query(con, "SELECT r.a, s.a FROM r,s WHERE r.a \% s.b = s.c")) {
        //     finish_with_error(con);
        // }
        
        if (mysql_query(con, "SELECT a FROM r WHERE r.a < r.b")) {
            finish_with_error(con);
        }
        
        end = stop_tsc(start);
        printf("%llu\n",end);
        result = mysql_store_result(con);
        mysql_free_result(result);
  
    }



    for(size_t i = 0; i < 10;++i){
       
        start = start_tsc();

        if (mysql_query(con, "SELECT a FROM r WHERE r.a < r.b")) {
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
