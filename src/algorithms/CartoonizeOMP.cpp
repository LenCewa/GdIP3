#include "CartoonizeOMP.h"

#include <omp.h>
#include <cmath>
#include <ctime>

// Globale Variablen
int OMPsig_d, OMPsig_r, OMPtau, OMPkernelSize;

void CartoonizeOMP::process(const Parameters &params, const Image &src, Image &dst)
{
	std::clock_t start, end, runtime;
	start = clock();
	float duration;

	// Variablen holen
	OMPkernelSize = params.kernelSize;
	OMPsig_d = params.sig_d;
	OMPsig_r = params.sig_r;
	OMPtau = params.tau;

	dst = src;
	/*Image bil = bilateralFilter(src);
	Image bin = binaryTraversion(src);
	dst = cartoonize(bil, bin);*/

	end = clock();
	runtime = end - start;
	duration = 1000 * (float)runtime / CLOCKS_PER_SEC;

	printf("Dauer: %f ms\n", duration);


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
