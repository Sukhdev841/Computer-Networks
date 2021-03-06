/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "sum.h"


void
arraysum_1(char *host,double_array d)
{
	CLIENT *clnt;
	singal_array  *result_1;
	double_array  sum_array_1_arg;
	int i;

#ifndef	DEBUG
	clnt = clnt_create (host, ARRAYSUM, ARRAYSUM_1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

   sum_array_1_arg.size = d.size;
   sum_array_1_arg.array1 = (int*)(malloc(sizeof(int)*d.size));
   sum_array_1_arg.array1 = (int*)(malloc(sizeof(int)*d.size));
   for(i=0;i<d.size;i++)
	{
	   sum_array_1_arg.array1[i] = d.array1[i];
	   sum_array_1_arg.array2[i] = d.array2[i];
	}

	result_1 = sum_array_1(&sum_array_1_arg, clnt);
	if (result_1 == (singal_array *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else
	{ 
	   printf("\nResult\n");
	   for(i=0;i<result_1->size;i++)
	   {
	      printf("%d ",result_1->array[i]);
	   }
	   printf("\n");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;
	double_array d;
	int i;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	printf("\nSize = ");
	scanf("%d",&d.size);
	d.array1 = (int*)(malloc(sizeof(int)*d.size));
	d.array2 = (int*)(malloc(sizeof(int)*d.size));
	for(i=0;i<d.size;i++)
	{
	   scanf("%d",&d.array1[i]);
	}
	for(i=0;i<d.size;i++)
	{
	   scanf("%d",&d.array2[i]);
	}
	arraysum_1 (host,d);
exit (0);
}
