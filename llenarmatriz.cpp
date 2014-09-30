#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define lado	5
#define p 		0.99
int main ()
{
	int semilla;
	int aleatorio;
	int i,j;
	int unos=0;
	int total=lado*lado;
	int M[lado][lado];
	semilla = time(NULL); //hr del sistema como semilla
	srand(semilla);
	for (i=0;i<lado;i++)
	{
		for (j=0;j<lado;j++)
		{
			aleatorio=rand();
			if (aleatorio < RAND_MAX*p)
			{
				M[i][j]=1;
				unos++;
			}
			else
				M[i][j]=0;
		}
	}
	for (i=0;i<lado;i++)
	{
		for (j=0;j<lado;j++)
		{
			printf(" %d",M[i][j]);
		}
		printf("\n");
	}
	printf("\n tenemos %d unos de %d números\n\n",unos,total);
}


