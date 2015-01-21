/*
CSE 430- HW2 Group 36

Authors:
Vignesh Iyer 1207653833
Akhilesh Goud Aila 1207240953
Abha Upadhyay 1207062047

Program to perform matrix multiplication using threads.

*/

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#define N 2048 // max size
#define SLEEP 5

void *doyourpart(void *param);
int A[N][N],B[N][N],C[N][N];
//size of matrix1 and matrix2 respectively.  
int matrix1size,matrix2size;
int matrixSize(char *line);
//no of threads
int TT; 


int main(int argc, char** argv)
{
        int i;
        int j;

	    struct timeval time;
        gettimeofday(&time,NULL);
        double start = time.tv_sec + time.tv_usec/1000000.0;

        //file pointers to read matrix1, matrix2 and write result matrix
	    FILE *fpMat1, *fpMat2, *fpResult;

        //if toStdOut is 1 then display on terminal else write to a text file	
        int toStdOut;
	    int ele;
        char line[5001];  
    
        //check if output file name is provided or not.
	    if(argc == 5)
		{
            toStdOut = 0;
            TT = atoi(argv[4]); // convert char** to int
        }
	    else if(argc == 4)
		{
            toStdOut = 1;
            TT = atoi(argv[3]); // convert char** to int
        }

        //read matrix 1
	    fpMat1 = fopen(argv[1],"r");
	    if(fpMat1 == NULL)
	    {
		    printf("Cannot read matrix1%s\nEnd of program \n",argv[1]);
		    exit(1);
	    } 
        fgets(line,5000,fpMat1);
        matrix1size = matrixSize(line);
	    	

	    //read matrix 2
	    fpMat2 = fopen(argv[2],"r");
	    if(fpMat2 == NULL)
	    {
		    printf("Cannot read matrix2%s\nEnd of program \n",argv[2]);
		    exit(1);
	    }
        fgets(line,5000,fpMat2);
	    matrix2size = matrixSize(line); 
        
        // check if number of cols of 1st matrix equals no. of rows of 2nd matrix
	    if(matrix1size != matrix2size)
	    {
		    printf("Matrix1 and Matrix2 cannot be multiplied\n");
		    fclose(fpMat1);
		    fclose(fpMat2);		
		    exit(1);
	    } 

        //Initialize matrix 1
        rewind(fpMat1);
        for(i=0;i<matrix1size;i++)
            for(j=0;j<matrix1size;j++)
            {
                fscanf(fpMat1,"%d",&ele);
                A[i][j] = ele;            
            }

        //Initialize matrix 2
        rewind(fpMat2);
        for(i=0;i<matrix2size;i++)
            for(j=0;j<matrix2size;j++)
            {
                fscanf(fpMat2,"%d",&ele);
                B[i][j] = ele;            
            }

        fclose(fpMat1);
        fclose(fpMat2);

        int *parts;
        parts = malloc(TT*sizeof(int));
        if(parts == NULL)
        {
            printf("malloc failed\n");
            exit(1);
        }
        
        pthread_t *tid;
        tid = malloc(TT*sizeof(pthread_t));
        if(tid == NULL)
        {
            printf("malloc failed\n");
            exit(1);
        }

        pthread_attr_t pt_attribs;
	

        for (i=0; i<TT; i++)
        {
                parts[i] = i;
                pthread_attr_init(&pt_attribs);
                //create threads
                pthread_create(&(tid[i]), &pt_attribs, doyourpart, &(parts[i]));
        }

        if (SLEEP == 1) sleep(30);

        for (i=0; i<TT; i++)
                pthread_join(tid[i], NULL);

        if(toStdOut == 1)
        {
            //display on terminal
            for(i=0;i<matrix2size;i++)
            {
                for(j=0;j<matrix2size;j++)
                    printf("%d ",C[i][j]);            
                printf("\n");
            }
        }
        else
        {
            //write to a file
            fpResult = fopen(argv[3],"w");
            if(fpResult==NULL)
            {
                printf("Result file doesnot exist\nEnd of program\n");
                exit(1);
            }
            for(i=0;i<matrix2size;i++)
            {
                for(j=0;j<matrix2size;j++)
                    if(j==matrix2size-1)
                        fprintf(fpResult,"%d",C[i][j]);
                    else                
                        fprintf(fpResult,"%d ",C[i][j]);
                fprintf(fpResult,"\n");
            }
            fclose(fpResult);
        }
        //free memory
        free(parts);
        free(tid);
        gettimeofday(&time,NULL);
        double end = time.tv_sec + time.tv_usec/1000000.0;        
        printf("Turn around time = %f\n",end-start);
        return 0;
}


void *doyourpart(void *param)
{
    int part = *(int *)param;
    if(TT<=matrix2size)
    {
	    int lower = part*(matrix2size/TT);
	    int upper = (part+1)*(matrix2size/TT)-1;
        int i,j,k;
        for(i=lower;i<=upper;i++)
            for(j=0;j<N;j++)
            {
                C[i][j]=0;
                for(k=0;k<N;k++)
                {
                    C[i][j] = C[i][j]+A[i][k]*B[k][j];   
                }
            }
    }
    else
    {
        //no of threads greater than matrix size
        int m = (int)matrix2size/sqrt((double)TT);
        int i,j,k;
        for(i= (part*m/matrix2size)*m;i<(part*m/matrix2size+1)*m;i++)
            for(j=(part%(matrix2size/m))*m;j<((part%(matrix2size/m))+1)*m;j++)                    
            {
                C[i][j]=0;
                for(k=0;k<N;k++)
                    C[i][j] = C[i][j]+ A[i][k]*B[k][j]; 
            }  
    }
    pthread_exit(0);
}

//this function returns the size of a matrix
int matrixSize(char *line)
{
	int count=0,i,length;
	//Enter code here
	length = strlen(line);
	for(i=0;i<length;i++)
	{	
        if(line[i] == ' ')
			count++;		
        else if(line[i] == '\n')
            break;
	}
	return (count+1);
}



