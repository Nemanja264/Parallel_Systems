%%cuda
#include <stdio.h>
#define BLOCKSIZE 256
#define RADIUS 2

__global__ void kernelFunc(float *a, float *b, float *c, int total, int n) {
   int stride = gridDim.x*blockDim.x;
  for(int idx=blockIdx.x*blockDim.x+threadIdx.x; idx<total; idx+=stride)
    {
      int i=idx/n;
      int j=idx%n;
      c[i*n+j] = a[i*n+j]-2*b[j*n+i];
    }
}

__global__ void kernel2(float* v, float* C, int n)
{
  int stride = gridDim.x*blockDim.x;;
  for(int col=blockIdx.x*blockDim.x+threadIdx.x; col<n; col+=stride)
    {
      float sum =0;
      for(int row=0; row<n;row++)
        {
          sum+=C[row*n+col];
        }
      v[col]=sum/n;
    }
}

int main() {
  int n = 4;
  float h_A[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    float h_B[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

  float* V = (float*)malloc(sizeof(float)*n);

  size_t sizeM = n*n*sizeof(float);
  size_t sizeV = n*sizeof(float);
  float* d_A, *d_B, *d_C, *d_V;

  cudaMalloc((void**)&d_A, sizeM);
  cudaMalloc((void**)&d_B, sizeM);
  cudaMalloc((void**)&d_C, sizeM);
  cudaMalloc((void**)&d_V, sizeV);

  cudaMemcpy(d_A, h_A, sizeM, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, h_B, sizeM, cudaMemcpyHostToDevice);

  int numBlocks = (n*n+BLOCKSIZE-1)/BLOCKSIZE;
  if(numBlocks > 256) numBlocks = 256;
  kernelFunc<<<numBlocks, BLOCKSIZE>>>(d_A, d_B, d_C, n*n, n);

  numBlocks = (n+BLOCKSIZE-1)/BLOCKSIZE;
  if(numBlocks > 256) numBlocks = 256;
  kernel2<<<numBlocks, BLOCKSIZE>>>(d_V, d_C, n);
  cudaMemcpy(V, d_V, sizeV, cudaMemcpyDeviceToHost);


  for (int i = 0; i < n; i++)
        printf("%.2f\n", V[i]);
  
  cudaFree(d_A); cudaFree(d_B); cudaFree(d_C); cudaFree(d_V);
  free(V);
  return 0;
}