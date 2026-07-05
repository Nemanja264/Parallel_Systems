%%cuda
#include <stdio.h>
#define BLOCKSIZE 256
#define RADIUS 2

__constant__ char d_Seq[4];

__global__ void kernelFunc(char *a, int n, int m, int* b) {
   int gid = blockDim.x*blockIdx.x+threadIdx.x;
  
  if(gid>=n)
    return;
  if(gid>n-m)
  {
    b[gid] = 0;
      return;
  }

  int match=1;
  for(int i=0;i<m;i++)
    {
      if(a[gid+i]!=d_Seq[i])
      {
        match=0;
        break;
      }
    }
    b[gid]=match;
}



int main() {
  char h_A[] = "AUGCUUAGCAUGCAUGCGGAUGC";
    char h_P[] = "AUGC";
   int n = strlen(h_A);
  int m = strlen(h_P);
  char* d_A;
  int* d_B;
    int* h_B = (int*)malloc(sizeof(int)*n);


  cudaMemcpyToSymbol(d_Seq, h_P, m);
  cudaMalloc((void**)&d_A, n);
    cudaMalloc((void**)&d_B, n*sizeof(int));
  cudaMemcpy(d_A, h_A, n, cudaMemcpyHostToDevice);

  kernelFunc<<<(n+BLOCKSIZE-1)/BLOCKSIZE, BLOCKSIZE>>>(d_A, n, m, d_B);
    cudaMemcpy(h_B, d_B, sizeof(int)*n, cudaMemcpyDeviceToHost);


  for (int i = 0; i < n; i++)
        printf("%d", h_B[i]);
    printf("\n");

  cudaFree(d_A); cudaFree(d_B);
  free(h_B);
   
    
  return 0;
}