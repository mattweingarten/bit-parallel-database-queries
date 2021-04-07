#include <cstdio>             
#include <math.h>
#include <stdlib.h>

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
void get_table(const char* file,size_t rows, size_t cols){
	FILE *fp;
	fp = fopen(file, "r");
	char buf[128];
	for(int i = 0; i < rows * cols; ++i){
		getValue(buf,fp);
		printf("%s\n",buf);
	}
}


enum DataTypes{INDEX,INT, FLOAT};

struct DataConfig{
	int N;
	int featurN;
	DataTypes* dTypes;
};

DataConfig parseDataConfigFile(const char* file){
	FILE *fp;
	fp = fopen(file, "r");
	char buf[128];
	char llc = '\0';
	char lc = '\0';
	int N = -1;
	int max = 0;
	while(llc != EOF && max < 5){
		max++;
		fgets(buf,128,fp);
		// ignore empty or comment line
		if(buf[0] == '#')
			continue;

		int ccount = 0;//comma count
		
		char currentword[13];
		for(int i = 0; i < 128;i++){
			if(buf[i] == ',')
				ccount++;
			if(buf[i] == EOF){
				llc = EOF;
				break;
			}
			if(buf[i] == '\n')
				break;
			
		}

		DataTypes dt[ccount];
		int dtx = 0;
		
		ccount++; //since there are only comma between the datafields

		int k = 0;
		for(int i = 0; i<=ccount;i++){
			int j = -1;
			lc = '\0';
			while(j < 12 && lc !=',' && lc != EOF && lc != '\n'){
				j++;
				currentword[j] = lc = buf[k];
				k++;
			}
			
			if(currentword[0] == 'I') {
				if (j > 1 && currentword[2] == 'D'){
					//// we have a index
					dt[dtx] = INDEX;
					dtx++;
					}
				else{
					/// we have a INT
					dt[dtx] = INT;
					dtx++;
				}
			}
			else if(currentword[0] =='F'){
				dt[dtx] = FLOAT;
				dtx++;
			}
			else if(N == -1 && currentword[0] <= '9' && currentword[0] >= '0'){
				//try to get N from currentword
				//if it is a number and not the first number to appear it will just be ignored
				int nn = 0;
				
				int p = 1;

				for(int jj = j-1; 0<=jj;jj--){
				//for(int jj = 0; jj<j;jj++){
					if( currentword[jj]< '0' && currentword[jj] > '9'){
						printf("parsing error\n");
						break;
					}
					

					nn += ((int)currentword[jj] - '0' )* p;
					p *= 10;

				}
				N = nn;
			}
		}
		
		// construct return
		if(dtx != 0){ // use this to find out if the line had content
			DataConfig dc;
			dc.N = N;
			dc.featurN = dtx;
			dc.dTypes =(DataTypes *) malloc(sizeof(int)*dtx);
			
			for(int i = 0; i < dtx; i++){
				dc.dTypes[i] = dt[i];
			}

			fclose(fp);	
			return dc;

		}

	}
	fclose(fp);	
	printf("something went wrong parsing the DataConfig file \n");
	DataConfig dc;
	dc.N = 0;
	dc.featurN = 0;
	return dc;
	
}


struct basicData{
	bool* featureisfloat;
	int* featureindex;
	
	int** int_features;
	float** float_features;

	DataConfig dc;
};

basicData getBasicDatafromfile(DataConfig dc, const char* file){
	

	//setup struct as container for the data
	basicData data;
	
	data.dc = dc;
	data.featureisfloat = (bool*) malloc(sizeof(bool) * dc.featurN);
	data.featureindex = (int*) malloc(sizeof(int) * dc.featurN);
	
	int intcount = 0;
	int floatcount = 0;


	for(int i = 0; i < data.dc.featurN; i++){
		switch(data.dc.dTypes[i] )
		{
			case INDEX:
				data.featureindex[i] = intcount;
				data.featureisfloat[i] = false;
				intcount++;
				break;
			case INT:
				data.featureindex[i] = intcount;
				data.featureisfloat[i] = false;
				intcount++;
				break;
			case FLOAT:
				data.featureindex[i] = floatcount;
				data.featureisfloat[i] = true;
				floatcount++;
				break;
			default:
				printf("error- unknown data type\n");
		}
	}
	

	// allocate memory
	data.int_features = (int**) malloc(sizeof(int*) * intcount);
	data.float_features = (float**) malloc(sizeof(float*) * floatcount);

	for(int i = 0; i < intcount; i++){
		data.int_features[i] = (int*) malloc(sizeof(int) * data.dc.N);

	}
	for(int i = 0; i < floatcount; i++){
		data.float_features[i] = (float*) malloc(sizeof(float) * data.dc.N);

	}

	/// read file
	
	FILE *fp;
	fp = fopen(file, "r");
	char buf[128];

	for(int r = 0; r < data.dc.N; r++){
		for(int c = 0; c < data.dc.featurN; c++){
			getValue(buf,fp);
			if(data.featureisfloat[c]){
				(data.float_features[data.featureindex[c]])[r] = atof(buf);
			}
			else
			{
				(data.int_features[data.featureindex[c]])[r] = atoi(buf);
			}
		}
	}
	
	// return data
	return data;
}
