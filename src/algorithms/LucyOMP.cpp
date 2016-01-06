#include "LucyOMP.h"

#include <omp.h>
#include <cmath>
#include <ctime>

void LucyOMP::process(const Parameters &params, const Image &src, Image &dst)
{
	std::clock_t start, end, runtime;
	start = clock();
	float duration;

	const int n = params.paramL;
	double kernel = 51.0;
	dst = src;
	Image b, u, d;
	for (int i = 0; i < n; i++) {
		/*b = filter(kernel, dst);
		u = elementwiseArithmeticOperation(src, b, '/');
		d = filter(kernel, u);
		dst = elementwiseArithmeticOperation(dst, d, '*');*/
	}
	end = clock();
	runtime = end - start;
	duration = 1000 * (float)runtime / CLOCKS_PER_SEC;

	printf("Iterationen: %i\n", params.paramL);
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