
#include "include/mlweaving/mlweaving.h"
#include "include/ASL/load.h"
#include <cstdlib>
#include <cstdio>             

int main(int argc, char **argv) {


	char * file = argv[1];
	printf("lines: %d\n",count_rows(file));
	printf("columns: %d\n",count_columns(file));
	get_table(file,count_rows(file),count_columns(file));

}