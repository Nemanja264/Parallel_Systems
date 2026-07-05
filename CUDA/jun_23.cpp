%%cuda
#include <stdio.h>
#include <limits.h>
#define BLOCKSIZE 256
#define RADIUS 2


__global__ void kernelFunc(int *a, int n, int* min) {
   int gid = blockDim.x*blockIdx.x+threadIdx.x;
  int stride = blockDim.x*gridDim.x;

  if(gid>=n)
    return;

  atomicMin(min, a[gid*n+gid]);
}

int main() {
  int n = 4;
    int min = INT_MAX;
    size_t sizeM = n*n * sizeof(int);

    int h_A[16] = {
        7, 2, 3, 4,
        5, 3, 7, 8,
        9, 1, 8, 2,
        6, 4, 5, 4
    };

  int *d_A, *d_min;
  cudaMalloc((void**)&d_A, sizeM);
    cudaMalloc((void**)&d_min, sizeof(int));
  cudaMemcpy(d_A, h_A, sizeM, cudaMemcpyHostToDevice);
    cudaMemcpy(d_min, &min, sizeof(int), cudaMemcpyHostToDevice);
  
  kernelFunc<<<(n+BLOCKSIZE-1)/BLOCKSIZE,BLOCKSIZE>>>(d_A,n,d_min);
  cudaMemcpy(&min, d_min, sizeof(int), cudaMemcpyDeviceToHost);

  printf("MINIMUM ON DIAGONAL IS: %d\n", min);
  cudaFree(d_A); cudaFree(d_min);

  return 0;
}