%%cuda
#include <stdio.h>
#define BLOCKSIZE 256
#define RADIUS 2

__global__ void kernelFunc(float *a, float *b, int *c, int n, float T) {
    __shared__ float tile[BLOCKSIZE+2*RADIUS];
  int tid = threadIdx.x;
  int gid = threadIdx.x+blockDim.x*blockIdx.x;

  if(gid<n)
    tile[tid+RADIUS] = a[gid];
  if(tid<RADIUS)
  {
      int left = gid-RADIUS;
      tile[tid] = (left>=0) ? a[left] : 0;
  }
  if(tid >= blockDim.x - RADIUS)
  {
    int right = gid+RADIUS;
    tile[tid+2*RADIUS]=(right<n) ? a[right] : 0;
  }
  __syncthreads();

  if(gid>=n) return;

  float sum=0;
  int cnt=0;
  for(int k=-RADIUS; k<=RADIUS;k++)
    {
      int idx = gid+k;
      if(idx>=0 && idx < n)
      {
        sum+=tile[tid+RADIUS+k];
        cnt++;
      }
    }
  float avg = sum/cnt;
  b[gid] = avg;
  c[gid] = (avg>=T) ? gid : -1;
}

int main() {
  int n = 20;
  float A[20] = {1.1f,  2.2f,  3.3f,  4.4f,  5.5f, 
        6.6f,  7.7f,  8.8f,  9.9f,  10.0f,
        11.1f, 12.2f, 13.3f, 14.4f, 15.5f, 
        16.6f, 17.7f, 18.8f, 19.9f, 20.0f};
  float* B = (float*)malloc(sizeof(float)*n);
  float T=10;
  int* C = (int*)malloc(sizeof(int)*n);

  float* d_A, *d_B;
  int* d_C;

  cudaMalloc((void**)&d_A, n*sizeof(float));
  cudaMalloc((void**)&d_B, n*sizeof(float));
  cudaMalloc((void**)&d_C, n*sizeof(int));
  cudaMemcpy(d_A, A, n*sizeof(float), cudaMemcpyHostToDevice);
  int numBlocks = (n+BLOCKSIZE-1)/BLOCKSIZE;
  kernelFunc<<<numBlocks, BLOCKSIZE>>>(d_A, d_B, d_C, n, T);

  cudaMemcpy(B, d_B, sizeof(float)*n, cudaMemcpyDeviceToHost);
  cudaMemcpy(C, d_C, sizeof(float)*n, cudaMemcpyDeviceToHost);

  for (int i = 0; i < n; i++)
        printf("%.2f %d\n", B[i], C[i]);
  
  cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
  free(B); free(C);
    
  return 0;
}