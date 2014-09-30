/*
  Percolacion en una red Cuadrada.
  Algoritmo de Labelling: Hoshen - Kopelman
  (Stauffer)
  ///////////////////////////////////////////////////////////////
  sitioFila[LADO+1]   : vector de etiquetado de lineas
  masa_badlabel[LADO*LADO]     : vector de almacenamiento de masas de clusters 
                        (si su contenido es > 0) o de etiquetas equivocadas
			(contenido < 0)
  //////////////////////////////////////////////////////////////
*/ 

#define PANTALLA 0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LADO    4 
#define LADO2   16
#define P       0.4

#define VACIO   200000
#define VACIO2  400000

int main()
{
  int etiqueta[LADO2]; 
  int red[LADO+1][LADO+1];
  int i,j;
  int m,k;
  unsigned int uin;

 
  /* Semilla del generador de numeros al azar */
  uin = time(NULL);
//  uin =1031755806; 
  srand(uin);
  fprintf(stdout," semilla %d\n ",uin);

  /* lleno la red */
  k=0;
  for(i=1;i<LADO+1;i++)
   for(j=1;j<LADO+1;j++)
   { 
    etiqueta[k++] = 0;
    if (rand()<=(P*RAND_MAX))
     red[i][j] = 1 ;
    else 
     red[i][j] = VACIO;
//    fprintf(stdout,"%d\n",red[i][j]);
   } 
  
  /* La fila:0 y la columna:0 las fijo como vacias */
  for(j=0;j<LADO+1;j++)
  {
    red[0][j] = VACIO;
    red[j][0] = VACIO;
   } 

 m = BuscaCluster(red,etiqueta,LADO); 

 fprintf(stdout,"cantidad de clusters m:%d\n",m);

 for(i=0;i<LADO+1;i++)
 {
  for(j=0;j<LADO+1;j++)
   if(red[i][j]==0 || red[i][j]==VACIO)
    fprintf(stdout,"%d ",0);
   else
    fprintf(stdout,"%d ",red[i][j]);
  fprintf(stdout,"\n");
 } 
  
  fprintf(stdout,"\n");
  for(i=1;i<LADO2;i++)
    if(etiqueta[i]>0 && etiqueta[i]!=VACIO)
      fprintf(stdout," %d ",etiqueta[i]);
  fprintf(stdout,"\n");  

  return 1;
}
  
/*********************************************************
 * Rutina que busca clusters en una red de percolacion 2D
 * usando el algorimo de Hoshen-Kopelman.
 *********************************************************
 * INPUT: 
 *      int *nodo: direccion del primer elemento
 *                 del arreglo bidimensional que almacena el
 *                 estado de ocupacion de la red, que puede ser
 *                  VACIO o 1
 *      int *vlabel: direccion del primer elemento del
 *                   arreglo que almacenara la informacion
 *                   de la masa de cada cluster. Su contenido
 *                   es modificado por la subrutina.
 *       int l: lado de la red cuadrada.
 *
 * OUTPUT:
 *      int  -> multiplicidad del evento (nro de fragmentos) 
 *********************************************************
 * EJEMPLO:
 *
 * En el analisis del siguiente evento:
 * 
 * | || || || || |
 * | || ||1|| ||2|   multiplicidad : 4
 * | ||3|| || || |           
 * | || ||4|| || |   vlabel[1] vlabel[2] vlabel[3] vlabel[4]
 * | ||4||4||4|| |       1        1           1        4
 *
 *********************************************************/

int BuscaCluster(int *nodo, int *vlabel, int l)
{
 float *sitioFila;  
 int labelOld, labelLeft, labelTop, label;
 int ocupadoLeft, ocupadoTop;
 int i,j;
 int index;
 int masa;
 int iaux;
 int m;


 index = 0;
 m=0;
 sitioFila = (float *)calloc(l+1,sizeof(float));
 for(i=0;i<l+1;i++)
   sitioFila[i] = VACIO;
  
 for(i=1;i<l+1;i++)
 {
    labelOld = VACIO;
    for(j=1;j<LADO+1;j++)
	{
	 labelLeft = sitioFila[j-1]; /* el vecino izq. es el que visite antes */

     /* sitio ocupado? */ 
	 if(nodo[i*(l+1)+j] != VACIO)   
	 {
	  labelTop  = sitioFila[j]; /* sitioFila[j] tiene el valor que corresponde
	                			 al sitio j de la fila i-1 */
    
	  /* Vecinos Ocupados? */
	  if((labelLeft + labelTop) == VACIO2)
	  { 
	   /* el sitio no tiene vecinos ocupados (es un cluster nuevo) */ 
       index++;
       m++;
	   sitioFila[j] = index;
	   vlabel[index] = 1;
	   nodo[i*(l+1)+j] = index;
	  }
	  else  /* Re-clasifico a los vecinos ocupados */
	  {
		  ocupadoTop  = (labelTop != VACIO);
		  ocupadoLeft = (labelLeft!= VACIO);

		  /* necesito re-etiquetar al vecino de arriba? */
		  if(ocupadoTop && vlabel[labelTop]<0)
		    while(vlabel[labelTop] < 0)
		      labelTop = -vlabel[labelTop];
		  
		  /* necesito re-etiquetar al vecino de la izquierda? */
		  if(ocupadoLeft && vlabel[labelLeft]<0)
		    while(vlabel[labelLeft] < 0)
		      labelLeft = -vlabel[labelLeft];

		  /* calculo la masa del cluster al que pertenece el sitio */ 
		  masa = 1; 
		  if(ocupadoTop)
		    masa += vlabel[labelTop];
		  if(ocupadoLeft && (labelTop!=labelLeft)) 
		    masa += vlabel[labelLeft];
     
		  if(labelLeft < labelTop)
		    label = labelLeft;    
		  else 
		    label = labelTop;

		  vlabel[label] = masa; 
		  nodo[i*(l+1)+j] = label;
       
		  /* 
		     la masa de los vecinos ya la considere en 'vlabel[label]'
		     asi que si sus etiquetas no coincidian las pongo como
		     etiquetas equivocadas en el elemento correspondiente del 
		     vector vlabel (valores negativos a la etiqueta de referencia)
		     */  
		  if(ocupadoTop && (labelTop!=label))
		  {
		    vlabel[labelTop] = -label;
		    nodo[(i-1)*(l+1)+j] = label;
            m--;
		  } 
		  if(ocupadoLeft && (labelLeft!=label))
		  {
		    nodo[i*(l+1)+j-1] = label;
		    vlabel[labelLeft] = -label;
            m--;
		  }  

		  sitioFila[j] = label;
	  }
	 }
	 else /* sitio no ocupado */
	  sitioFila[j] = VACIO;
	}
 }

 /* re-etiquetado */
 for(i=1;i<l+1;i++)
  for(j=1;j<l+1;j++)
  {
   iaux=nodo[i*(l+1)+j];
   if (iaux != VACIO)
   {
    label = vlabel[iaux];
    if(label < 0)
     nodo[i*(l+1)+j] = -label;
   }  
  }
 return m; 
}
