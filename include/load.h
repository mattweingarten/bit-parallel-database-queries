#ifndef LOAD
#define LOAD

#include <stdio.h>


int count_rows(const char* file){
	FILE *fp;
	fp = fopen(file, "r");
	char c = fgetc(fp);
	int lines = 0;
	while(c != EOF){
		if(c == '\n'){
			lines++;
		}
		c = fgetc(fp);
	}
	fclose(fp);
	return lines;
}

int count_columns(const char* file){
	FILE *fp;
	fp = fopen(file,"r");
	char c = fgetc(fp);
	int columns = 1;
	while(c != EOF && c != '\n'){
		if(c == ','){
			columns++;
		
		}
		c = fgetc(fp);
	}
	fclose(fp);
	return columns;
}


void getValue(char * buf,FILE *fp){
	char c = fgetc(fp);
	int count = 0;
	while(c != EOF && c != ',' && c != '\n'){
		buf[count] = c;
		c = fgetc(fp);
		count++;
	}
	buf[count] = '\0';
}


// TODO: ADD type information, add malloc, return malloc pointer.
// How do we store this into MLWEAVE format? -> can we use the 
void* get_table(const char* file,size_t rows, size_t cols){
	FILE *fp;
	fp = fopen(file, "r");
	char buf[128];
	for(int i = 0; i < rows * cols; ++i){
		getValue(buf,fp);
		printf("%s\n",buf);
	}
	return NULL;
}

#endif