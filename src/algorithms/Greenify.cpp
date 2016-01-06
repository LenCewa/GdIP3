#include "Greenify.h"

#include <omp.h>
#include <cmath>

void Greenify::process(const Parameters &params, const Image &src, Image &dst)
{
   //************************************************************************
   // Access image data
   //************************************************************************
   dst = src;

   // get dimensions
   const int M = src.height();
   const int N = src.width();

   // traverse pixels
   for (int y = 0; y < M; ++y) {
      for (int x = 0; x < N; ++x) {

         const Pixel &pixel = src[y][x];

         double red   = pixel.r;
         double green = pixel.g;
         double blue  = pixel.b;

         green = std::min(255.0, green + 50.0); // increase greeniness

         dst[y][x] = Pixel(red, green, blue);
      }
   }

   //************************************************************************
   // How to use parameters from gui
   //************************************************************************
   printf("Param1 (bool): %i\n", params.param1);;
   printf("Param2 (int): %i\n", params.param2);
   printf("Param3 (double): %f\n", params.param3);


   //************************************************************************
   // OPEN MP
   //************************************************************************
   int nthreads, tid;
   /* Fork a team of threads giving them their own copies of variables */
   #pragma omp parallel private(nthreads, tid)
   {
      /* Obtain thread number */
      tid = omp_get_thread_num();
      printf("Hello World from thread = %d\n", tid);

      // Only master thread does this
      if (tid == 0) {
         nthreads = omp_get_num_threads();
         printf("Number of threads = %d\n", nthreads);
      }
   }  /* All threads join master thread and disband */



}
