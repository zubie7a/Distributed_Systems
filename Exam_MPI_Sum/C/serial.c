#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 int P1;
 int P2;
 char OP;


void procesarText(char *text) {

char sp1[100];
char sp2[100];

int i=1;

while (text[i]!='=') {
 if (text[i]=='+' || text[i]=='-' || text[i]=='*' || text[i]=='/') {
    OP = text[i];
    strncpy(sp1, text, i);
    strncpy(sp2, text+i+1, strlen(text)-1);
    break;
 } else
    i++;
}

P1 = atoi(sp1);
P2 = atoi(sp2);



}

int operacion(int p1, char op, int p2) {
int result=-1;
if (op=='+')
	result = p1+p2;
else if (op=='-')
	result = p1-p2;
else if (op=='*')
	result = p1*p2;
else if (op=='/')
	result = p1+p2;
return result;
}
 
int main()
{
 	FILE *archivo_in;
	FILE *archivo_out;
 
 	char linea_in[100];
	char linea_out[100];

        int res;
 
 	archivo_in = fopen("data_in.txt","r");
	archivo_out = fopen("data_out.txt","w");
 
 	if (archivo_in == NULL || archivo_out == NULL)
 		exit(1);
 
 	while (feof(archivo_in) == 0)
 	{
 		fgets(linea_in,100,archivo_in);
 		procesarText(linea_in);
                res = operacion(P1,OP,P2);
		sprintf(linea_out,"%d%c%d=%d\n",P1, OP, P2,res);
		fputs(linea_out, archivo_out);	
 	}

        fclose(archivo_in);
	fclose(archivo_out);
	return 0;
}
