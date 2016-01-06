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
			b = filter(kernel, dst);
			u = elementwiseArithmeticOperation(src, b, '/');
			d = filter(kernel, u);
			dst = elementwiseArithmeticOperation(dst, d, '*');
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

Image LucyOMP::filter(double kernel, const Image &src)
{
	Image dst = src; // TOOD Abändern zu dst = Image(height, width); 
	// Dimension holen
	const int height = src.height();
	const int width = src.width();

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (!inRange(x, width)) {
				dst[y][x] = Pixel(255.0 / kernel, 255.0 / kernel, 255.0 / kernel);
				continue;
			}
			dst[y][x] = kernelOperation(kernel, src, y, x);
		}
	}
	
	return dst;
}

// Checkt ob rechts und links vom Pixel mindestens 25 weitere Pixel sind um in Range zu sein
bool LucyOMP::inRange(int x, const int width) { return (x - 25 < 0 || x + 25 > width) ? false : true; }

// Wendet den 1 x 51 Kernel auf den gegebenen Pixel an und gibt die Adresse des modifizierten Pixel zurück
Pixel LucyOMP::kernelOperation(double kernel, const Image &src, int y, int x)
{
	double r = 0.0, g = 0.0, b = 0.0;
	// Arithm. Mittel der 25 rechts und linksstehnden Pixel berechnen
	for (int w = x - 25; w < /*<= ?!*/ x + 25; w++) {
		const Pixel &pixel = src[y][w];
		r += pixel.r / kernel;
		g += pixel.g / kernel;
		b += pixel.b / kernel;
	}
	return Pixel(r, g, b);
}

// Wendet für jedes Pixel (Elementweise) die angewendete Operation op durch. Entweder (/) oder (*)
Image LucyOMP::elementwiseArithmeticOperation(const Image &left, const Image &right, char op) {
	// Dimension holen
	const int height = left.height();
	const int width = right.width();

	double r = 0.0, g = 0.0, b = 0.0;
	Image dst = Image(height, width);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {

			const Pixel &gPixel = left[y][x];
			const Pixel &bPixel = right[y][x];

			if (op == '/') {
				r = (gPixel.r / bPixel.r > 255) ? 255 : gPixel.r / bPixel.r;
				g = (gPixel.g / bPixel.g > 255) ? 255 : gPixel.g / bPixel.g;
				b = (gPixel.b / bPixel.b > 255) ? 255 : gPixel.b / bPixel.b;
			}
			// Es ist eine elementweise Multiplikation
			else {
				r = (gPixel.r * bPixel.r > 255) ? 255 : gPixel.r * bPixel.r;
				g = (gPixel.g * bPixel.g > 255) ? 255 : gPixel.g * bPixel.g;
				b = (gPixel.b * bPixel.b > 255) ? 255 : gPixel.b * bPixel.b;
			}

			dst[y][x] = Pixel(r, g, b);
		}
	}
	return dst;
}