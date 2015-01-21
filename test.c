#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
    int N = atoi(argv[1]);
printf("matrix order is %d\n",N);
    FILE *fp;
    fp = fopen("matrix1.rtf","w");
    int i,j;
    if(fp != NULL)
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
                if(j!=N-1)
                    fprintf(fp,"1 ");
                else
                    fprintf(fp,"1");
            fprintf(fp,"\n");
        }
        fclose(fp);        
    }

    fp = fopen("matrix2.rtf","w");
    if(fp != NULL)
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
                if(j!=N-1)
                    fprintf(fp,"1 ");
                else
                    fprintf(fp,"1");
            fprintf(fp,"\n");
        }
        fclose(fp);        
    }
    
}
