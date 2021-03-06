#include <algorithm> // for std::max
#include <cstdio>
#include <chrono>
#include <iostream>
#include <thread>
#include <random>
#include <cmath>
#include "tbb/task_scheduler_init.h"
#include "tbb/flow_graph.h" 


int M = 40000, N = 40000;
int B = 160;
int MB = (M/B) + (M%B>0);
int NB = (N/B) + (N%B>0);

double **matrix {nullptr};

// nominal operations
double calc(double v0, double v1) {
  return (v0 == v1) ? std::pow(v0/v1, 4.0f) : std::max(v0,v1);
}

// initialize the matrix
void init_matrix(){
  matrix = new double *[M];
  for ( int i = 0; i < M; ++i ) matrix[i] = new double [N];
  for(int i=0; i<M; ++i){
    for(int j=0; j<N ; ++j){
      matrix[i][j] = i*N + j;
    }   
  }
}

// destroy the matrix
void destroy_matrix() {
  for ( int i = 0; i < M; ++i ) {
    delete [] matrix[i];
  }
  delete [] matrix;
}

// the wavefront computation
void wavefront() {

  using namespace tbb;
  using namespace tbb::flow;
    
  continue_node<continue_msg> ***node = new continue_node<continue_msg> **[MB];

  for ( int i = 0; i < MB; ++i ) {
    node[i] = new continue_node<continue_msg> *[NB];
  };

  graph g;

  matrix[M-1][N-1] = 0;

  for( int i=MB; --i>=0; ) {
    for( int j=NB; --j>=0; ) {
      node[i][j] =
        new continue_node<continue_msg>( g,
          [=]( const continue_msg& ) {
            int start_i = i*B;
            int end_i = (i*B+B > M) ? M : i*B+B;
            int start_j = j*B;
            int end_j = (j*B+B > N) ? N : j*B+B;
            for ( int ii = start_i; ii < end_i; ++ii ) {
              for ( int jj = start_j; jj < end_j; ++jj ) {
                double v0 = (ii == 0) ? 0 : matrix[ii-1][jj];
                double v1 = (jj == 0) ? 0 : matrix[ii][jj-1];
                matrix[ii][jj] = (ii==0 && jj==0) ? 1 : calc(v0,v1);
               }
            }
      });
      if(i+1 < MB) make_edge(*node[i][j], *node[i+1][j]);
      if(j+1 < NB) make_edge(*node[i][j], *node[i][j+1]);
    }
  }
  
  node[0][0]->try_put(continue_msg());
  g.wait_for_all();
  
  for(int i=0; i<MB; ++i) {
    for(int j=0; j<NB; ++j) {
     delete node[i][j];
    }
  }
}

// main function
int main(int argc, char *argv[]) {

  init_matrix();

  double result;
  tbb::task_scheduler_init init(std::thread::hardware_concurrency());

  auto beg = std::chrono::high_resolution_clock::now();
  wavefront();
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Intel TBB wavefront elapsed time: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count() 
            << " ms\n"
            << "result: " << matrix[M-1][N-1] << std::endl;

  destroy_matrix();

  return 0;
}



