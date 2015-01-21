#!/bin/sh

#typeset -a array
#array=( 1 4 16 64 256 )

chmod 777 matrixmult_process.c
chmod 777 matrixmult_thread.c

echo "execution started"

m=2048
#for m in 128 256 512 1024 2048
#do
  gcc test.c
  ./a.out $m
 for pt in 1 4 16 64 256
 do
 echo "no. of processess/threads used is $pt"
   for i in 1 2 3 4 5 6 7 8 9 10
   do
     gcc matrixmult.c -lm
     ./a.out matrix1.rtf matrix2.rtf result1.rtf $pt>>output3.txt

     gcc matrixmultthreads.c -lm -pthread
     ./a.out matrix1.rtf matrix2.rtf result2.rtf $pt>>output4.txt
   done
 done
#done
echo "your process and thread outputs are ready."
