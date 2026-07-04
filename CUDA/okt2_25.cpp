%%cuda
#include <stdio.h>
#define BLOCKSIZE 256

__global__ void kernelFunc(float *a, int m, int n, int* time_elapsed) {
   int gid = blockDim.x*blockIdx.x+threadIdx.x;

  int stride = gridDim.x*blockDim.x;
  for(int row=gid; row<m;row+=stride)
    {
      float prev=a[row*n+0];
      int count = 0;
      for(int col=1;col<n;col+=1)
        {
          float cur = a[row*n+col];
          if(cur<0.5 && prev>cur)
              count++;
          else
            count=0;
          prev = cur;
        }
      time_elapsed[row]=count;
    }
}

int main() {
 int M = 3, N = 8;
  float h_A[24] = {
        0.9, 0.8, 0.7, 0.6, 0.4, 0.3, 0.2, 0.1,
        0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
        0.4, 0.3, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1
    };
  float* d_A;
  int* h_T = (int*)malloc(M*sizeof(int));
  int* d_T;
  cudaMalloc((void**)&d_A, M*N*sizeof(float));
  cudaMalloc((void**)&d_T, M*sizeof(int));
  cudaMemcpy(d_A, h_A, M*N*sizeof(float),cudaMemcpyHostToDevice);
  
  kernelFunc<<<(M+BLOCKSIZE-1)/BLOCKSIZE, BLOCKSIZE>>>(d_A, M, N, d_T);
  cudaMemcpy(h_T, d_T, M*sizeof(int), cudaMemcpyDeviceToHost);


  for (int i = 0; i < M; i++)
        printf("Biljka %d: %d h\n", i, h_T[i]);
  
  cudaFree(d_A); cudaFree(d_T);
  free(h_T); 
    
  return 0;
}