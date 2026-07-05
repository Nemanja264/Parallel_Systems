%%cuda
#include <stdio.h>
#define BLOCKSIZE 256
#define MAXBLOCKS 256

#define RADIUS 2


__global__ void kernelFunc(int* a, int* b, int* c, int n) {
   int gid = blockDim.x*blockIdx.x+threadIdx.x;
  int stride = blockDim.x*gridDim.x;
  
  for(int idx=gid; idx<n*n; idx+=stride)
    {
      int i = idx/n;
      int j = idx%n;
      int av=a[i*n + j];
      int bv = b[j*n+i];
      c[i*n+j] = (av>bv) ? av : bv;
    }
}
__global__ void kernel2(int*c, int* v, int n)
{
  int gid = blockDim.x*blockIdx.x+threadIdx.x;
  int stride = blockDim.x*gridDim.x;
  
  for(int i=gid; i<n; i+=stride)
    {
      int min = c[i*n+0];
      for(int j=1;j<n;j++)
        {
          int curr = c[i*n+j];
          if(min>curr)
            min=curr;
        }
      v[i] = min;
    }
}


int main() {
  int n = 4;
    int total = n * n;
    size_t sizeM = total * sizeof(int);
    size_t sizeV = n * sizeof(int);

    int h_A[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    int h_B[16] = {
        16, 15, 14, 13,
        12, 11, 10, 9,
        8, 7, 6, 5,
        4, 3, 2, 1
    };
    int *h_V = (int*)malloc(sizeV);

    int *d_A, *d_B, *d_C, *d_V;

  cudaMalloc((void**)&d_A, sizeM);
    cudaMalloc((void**)&d_B, sizeM);
    cudaMalloc((void**)&d_C, sizeM);
    cudaMalloc((void**)&d_V, sizeV);
    cudaMemcpy(d_A, h_A, sizeM, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, sizeM, cudaMemcpyHostToDevice);


   int blocksC = (total + BLOCKSIZE - 1) / BLOCKSIZE;
    if (blocksC > MAXBLOCKS) blocksC = MAXBLOCKS;
    kernelFunc<<<blocksC, BLOCKSIZE>>>(d_A, d_B, d_C, n);

  int blocksV = (n + BLOCKSIZE - 1) / BLOCKSIZE;
    if (blocksV > MAXBLOCKS) blocksV = MAXBLOCKS;
    kernel2<<<blocksV, BLOCKSIZE>>>(d_C, d_V, n);

  cudaMemcpy(h_V, d_V, sizeV, cudaMemcpyDeviceToHost);

    for (int row = 0; row < n; row++)
        printf("%d\n", h_V[row]);

  cudaFree(d_A); cudaFree(d_B); cudaFree(d_C); cudaFree(d_V);
    free(h_V);
   
    
  return 0;
}