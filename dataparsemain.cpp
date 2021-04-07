
#include "include/mlweaving/mlweaving.h"
#include "include/ASL/parseConfig.h"
#include <cstdlib>
#include <cstdio>             

int main(int argc, char **argv) {


	char * configfile = argv[1];
	char * datafile = argv[2];
	//printf("lines: %d\n",count_rows(file));
	//printf("columns: %d\n",count_columns(file));
	//get_table(file,count_rows(file),count_columns(file));
	//
	

	// read DataConfig 
	DataConfig dc = parseDataConfigFile(configfile);

	// output DataConfig information
	printf("the loaded config file promises %i rows in the data file.\n",dc.N);
	printf("The Data Types of the features are: ");
	for(int i = 0;i < dc.featurN; i++){
		switch(dc.dTypes[i])
		{
			case INDEX:
				printf("INDEX(INT),");
				break;
			case INT:
				printf("INT,");
				break;
			case FLOAT:
				printf("FLOAT,");
				break;
			default:
				printf("error,");
		}
	}

	printf("starting to read data\n");
	/// reading in data
	//
	basicData data = getBasicDatafromfile(dc,datafile);

	printf("the second column are: \n" );
	for(int i = 0; i < dc.N; i++){
		if(data.featureisfloat[1]){
			printf("%f",data.float_features[data.featureindex[1]][i]);
		}
		else
		{
			printf("%d",data.int_features[data.featureindex[1]][i]);
		}
	}

	printf("\nDone");

	return 0;
}
