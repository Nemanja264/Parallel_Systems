%%cuda
#include <stdio.h>
#define BLOCKSIZE 256
#define RADIUS 2

__global__ void kernelFunc(float *a, int n, float alpha, int* count) {
  __shared__ float tile[BLOCKSIZE+2];
   int gid = blockDim.x*blockIdx.x+threadIdx.x;
  int tid = threadIdx.x;

  if(gid<n)
    tile[tid+1]=a[gid];
  if(tid==0 && blockIdx.x > 0)
    tile[tid] = a[gid-1];
  if(tid == blockDim.x-1 && gid+1<n)
    tile[tid+2]=a[gid+1];

  __syncthreads();

  if(gid>0 && gid<n-1)
  {
    float cur = tile[tid+1];
    if(cur>tile[tid] && cur>tile[tid+2] && cur>alpha)
      atomicAdd(count,1);
  }
}

int main() {
 int n = 12;
  float alpha = 0.5f;
  float h_A[12] = {0.1, 0.9, 0.2, 0.3, 0.8, 0.1, 0.4, 1.2, 0.3, 0.2, 0.95, 0.1};

  float* d_A;
  int count=0, *d_count;
  cudaMalloc((void**)&d_A, 12*sizeof(float));
  cudaMalloc((void**)&d_count, sizeof(int));
  cudaMemcpy(d_A, h_A, 12*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_count, &count, sizeof(int), cudaMemcpyHostToDevice);

  kernelFunc<<<(n+BLOCKSIZE-1)/BLOCKSIZE, BLOCKSIZE>>>(d_A, n, alpha, d_count);
  cudaMemcpy(&count, d_count, sizeof(int), cudaMemcpyDeviceToHost);

  printf("%d\n", count);


  cudaFree(d_A); cudaFree(d_count);
   
    
  return 0;
}