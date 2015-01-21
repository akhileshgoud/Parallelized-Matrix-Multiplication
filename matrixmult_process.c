/*
CSE 430- HW2 Group 36

Authors:
Vignesh Iyer 1207653833
Akhilesh Goud Aila 1207240953
Abha Upadhyay 1207062047

Program to perform matrix multiplication using process and shared memory.

*/

#include <sys/shm.h>
#include <sys/wait.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <time.h>
#include <math.h>


#define N 2048 // max memory size

int matrixSize(char *line);

int main(int argc,char** argv)
{
    
    struct timeval time;
    gettimeofday(&time,NULL);
    double start = time.tv_sec + time.tv_usec/1000000.0;

    //file pointers to read matrix1, matrix2 and write result matrix
	FILE *fpMat1, *fpMat2, *fpResult;
    //pointer to a pointer of ints to store 2D arrays 
	int **matrix1, **matrix2, **result;
    //size of matrix1 and matrix2 respectively.  
	int matrix1size,matrix2size;

    //if toStdOut is 1 then display on terminal else write to a text file	
    int toStdOut;
	char line[5001];  
    int ele;  
    
    typedef struct matrixstruct {
        
        int A[N][N];
        int B[N][N];
        int C[N][N];
        } matstr;

    matstr* Cms;
    int i,j,Cid,x,k;
    pid_t basepid;
    int P;
    
    //check if output file name is provided or not.
    if(argc == 5)
    {
        //output file present
        toStdOut = 0;
        P = atoi(argv[4]); //convert char** to int for no. of processes
    }
    else if(argc == 4)
    {
        //output file absent
        toStdOut = 1;
        P = atoi(argv[3]); //convert char** to int for no. of processes       
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

    //printf("%d %d\n",matrix1size,matrix2size);

    // check if number of cols of 1st matrix equals no. of rows of 2nd matrix
	if(matrix1size != matrix2size)
	{
		printf("Matrix1 and Matrix2 cannot be multiplied\n");
		fclose(fpMat1);
		fclose(fpMat2);		
		exit(1);
	}
        
    if((Cid = shmget(IPC_PRIVATE,sizeof(matstr),IPC_CREAT|0666))<0)
    {
        printf("Cannot allocate memory\n");
        exit(-1);
    }
    printf("Allocated %d, at id %d\n",(int)sizeof(matstr),Cid);


    //main thread attaching the shared memory
    if ((Cms = (matstr *) shmat(Cid, NULL, 0)) == (matstr *) -1)
    {   
        perror("Process shmat returned NULL\n");
        error(-1, errno, " ");
    }
    else
        printf("Main process attached the segment %d\n", Cid);

    //Initialize matrix 1
    rewind(fpMat1);
    for(i=0;i<matrix1size;i++)
        for(j=0;j<matrix1size;j++)
        {
            fscanf(fpMat1,"%d",&ele);
            Cms->A[i][j] = ele;            
        }

    //Initialize matrix 2
    rewind(fpMat2);
    for(i=0;i<matrix2size;i++)
        for(j=0;j<matrix2size;j++)
        {
            fscanf(fpMat2,"%d",&ele);
            Cms->B[i][j] = ele;            
        }     

    if (shmdt(Cms) == -1){
                        perror("shmdt returned -1\n");
                        error(-1, errno, " ");
            }
            else
                printf("Main process detached the segment %d\n", Cid);
    
    basepid = getpid();
    for(x=0;x<P;x++)
    {
        basepid = fork();
        if(basepid == 0)
        {
            // child process does work here
            if ((Cms = (matstr *) shmat(Cid, NULL, 0)) == (matstr *) -1)
            {   
                perror("Process shmat returned NULL\n");
                error(-1, errno, " ");
            }
            else
                printf("Process %d attached the segment %d\n", getpid(), Cid);
            
            if(P<=matrix2size)
            {            
                //matrix multiplication
                printf("Computing values from %d to %d\n",x*matrix2size/P,(x+1)*matrix2size/P-1);
                for(i=x*matrix2size/P;i<(x+1)*matrix2size/P;i++)
                    for(j=0;j<matrix2size;j++)
                    {
                        Cms->C[i][j]=0;
                        for(k=0;k<matrix2size;k++)
                            Cms->C[i][j] = Cms->C[i][j] + Cms->A[i][k]*Cms->B[k][j];
                    }
            }
            else
            {
                // no of processes is greater than matrix size
                int m = (int)matrix2size/sqrt((double)P);
                
                for(i= (x*m/matrix2size)*m;i<(x*m/matrix2size+1)*m;i++)
                    for(j=(x%(matrix2size/m))*m;j<((x%(matrix2size/m))+1)*m;j++)                    
                    {
                        //printf("printing values from %d,%d\n",i,j);
                        Cms->C[i][j]=0;
                        for(k=0;k<N;k++)
                            Cms->C[i][j] = Cms->C[i][j]+ Cms->A[i][k]*Cms->B[k][j]; 
                        //printf("printing values at %d\n",Cms->C[i][j]);                  
                    }  
                                           
                    
            }

            if (shmdt(Cms) == -1){
                        perror("shmdt returned -1\n");
                        error(-1, errno, " ");
            }
            else
                printf("Process %d detached the segment %d\n", getpid(), Cid);         

            
        }    
        else
        {
            wait(NULL);
            if(getpid()==basepid)
                for(i=1;i<N;i++)
                    wait(NULL);
            else
                exit(0);
        }
    }
    
    //main thread attaching the shared memory
    if ((Cms = (matstr *) shmat(Cid, NULL, 0)) == (matstr *) -1)
    {   
        perror("Process shmat returned NULL\n");
        error(-1, errno, " ");
    }
    else
        printf("Main process re-attached the segment %d\n", Cid);

    
    if(toStdOut == 1)
    {
        //display on terminal
        for(i=0;i<matrix2size;i++)
        {
            for(j=0;j<matrix2size;j++)
                printf("%d ",Cms->C[i][j]);            
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
                    fprintf(fpResult,"%d",Cms->C[i][j]);
                else
                    fprintf(fpResult,"%d ",Cms->C[i][j]);
            fprintf(fpResult,"\n");
        }
        fclose(fpResult);
    }    

    if (shmdt(Cms) == -1){
                        perror("shmdt returned -1\n");
                        error(-1, errno, " ");
            }
            else
                printf("Main process detached the segment %d\n", Cid);

     

    if (shmctl(Cid,IPC_RMID,NULL) == -1){
        perror("shmctl returned -1\n");
        error(-1, errno, " ");
    }

    fclose(fpMat1);
    fclose(fpMat2);

    gettimeofday(&time,NULL);
    double end = time.tv_sec + time.tv_usec/1000000.0;

    printf("Turn around time = %f\n",end-start);    

    return 0;
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


