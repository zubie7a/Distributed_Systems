#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
int main(int argc, char **argv) {
	FILE *archivo_out;
	char linea_out[71];
 
	archivo_out = fopen("data_in.txt","w");
 
 	if (archivo_out == NULL)
 		exit(1);
	
	int nro; 

	if (argc >0)
	  nro=atoi(argv[1]);
	else
           nro=1000;

	int i=0;
	int p1;
	int p2;
	char op;
	int valop;
 	while (i<nro) {
		p1 = rand() % 100 + 1;
		p2 = rand() % 100 + 1;
		valop= rand() % 256;
		if (valop>192)
			op='/';
		else if (valop>128)
			op='*';
		else if (valop>64)
			op='-';
		else 
			op='+';
		sprintf(linea_out,"%d%c%d=\n", p1, op, p2);
		fputs(linea_out, archivo_out);	
		i++;
 	}
	fclose(archivo_out);
	return 0;
}
