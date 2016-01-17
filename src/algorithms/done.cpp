//#include "LucyOMP.h"
//
//
//
//#include <omp.h>
//
//#include <cmath>
//
//#include <ctime>
//
//
//
//void LucyOMP::process(const Parameters &params, const Image &src, Image &dst)
//
//{
//
//	std::clock_t start, end, runtime;
//
//	start = clock();
//
//	float duration;
//
//	double s = omp_get_wtime();
//
//
//
//	const int n = params.paramL;
//
//	double kernel = 51.0;//params.kernelSizeLucy;
//
//	dst = src;
//
//	Image b, u, d;
//
//	for (int i = 0; i < n; i++) {
//
//		printf("Iteration %i\n", i);
//
//		b = filter(kernel, dst);
//
//		u = elementwiseArithmeticOperation(src, b, '/');
//
//		d = filter(kernel, u);
//
//		dst = elementwiseArithmeticOperation(dst, d, '*');
//
//	}
//
//	double e = omp_get_wtime() - s;
//
//	end = clock();
//
//	runtime = end - start;
//
//	duration = 1000 * (float)runtime / CLOCKS_PER_SEC;
//
//
//
//	printf("Iterationen: %i\n", params.paramL);
//
//	printf("Dauer: %lf ms\n", e);
//
//
//
//
//
//	//************************************************************************
//
//	// OPEN MP
//
//	//************************************************************************
//
//	int nthreads, tid;
//
//	/* Fork a team of threads giving them their own copies of variables */
//
//#pragma omp parallel private(nthreads, tid)
//
//	{
//
//		/* Obtain thread number */
//
//		tid = omp_get_thread_num();
//
//		printf("Hello World from thread = %d\n", tid);
//
//
//
//		// Only master thread does this
//
//		if (tid == 0) {
//
//			nthreads = omp_get_num_threads();
//
//			printf("Number of threads = %d\n", nthreads);
//
//		}
//
//	}  /* All threads join master thread and disband */
//
//
//
//
//
//}
//
//
//
//// Checkt ob rechts und links vom Pixel mindestens 25 weitere Pixel sind um in Range zu sein
//
//bool LucyOMP::inRange(int x, const int width) { return (x - 25 < 0 || x + 25 > width) ? false : true; }
//
//
//
//// Wendet den 1 x 51 Kernel auf den gegebenen Pixel an und gibt die Adresse des modifizierten Pixel zurück
//
//Pixel LucyOMP::kernelOperation(double kernel, const Image &src, int y, int x)
//
//{
//
//	double r = 0.0, g = 0.0, b = 0.0;
//
//	// Arithm. Mittel der 25 rechts und linksstehnden Pixel berechnen
//
//	for (int w = x - 25; w < /*<= ?!*/ x + 25; w++) {
//
//		const Pixel &pixel = src[y][w];
//
//		r += pixel.r / kernel;
//
//		g += pixel.g / kernel;
//
//		b += pixel.b / kernel;
//
//	}
//
//	return Pixel(r, g, b);
//
//}
//
//
//
//Image LucyOMP::filter(double kernel, const Image &src)
//
//{
//
//	const int height = src.height();
//
//	const int width = src.width();
//
//	Image dst = src;
//
//
//
//	omp_set_num_threads(2);
//
//
//
//#pragma omp parallel for
//
//	for (int y = 0; y < height; y++) {
//
//		double r = 0.0, g = 0.0, b = 0.0;
//
//#pragma omp parallel for schedule(static, 51)
//
//		for (int x = 0; x < width; x++) {
//
//
//
//			const Pixel &pixel = src[y][x];
//
//			r += pixel.r / kernel;
//
//			g += pixel.g / kernel;
//
//			b += pixel.b / kernel;
//
//
//
//			int count = (kernel - 1) - (x % 50);
//
//			if (x >= (kernel - 1)){
//
//				dst[y][x - ((int)kernel / 2)] = Pixel(r, g, b);
//
//				const Pixel &pix = src[y][x - (kernel - 1)];
//
//				r -= pix.r / kernel;
//
//				g -= pix.g / kernel;
//
//				b -= pix.b / kernel;
//
//			}
//
//			//Randbehandlung links
//
//			if (x < (kernel - 1)){
//
//				//weisse Pixel hinzurechnen
//
//				r += ((255.0 / kernel)*count);
//
//				g += ((255.0 / kernel)*count);
//
//				b += ((255.0 / kernel)*count);
//
//				dst[y][x] = Pixel(r, g, b);
//
//				//weisse Pixel wieder abziehen
//
//				r -= ((255.0 / kernel)*count);
//
//				g -= ((255.0 / kernel)*count);
//
//				b -= ((255.0 / kernel)*count);
//
//			}
//
//		}
//
//		//Randbehandlung rechts
//
//#pragma omp parallel for
//
//		for (int x = width - 25; x < width; x++)
//
//			dst[y][x] = rightRim(kernel, src, y, x, width);
//
//	}
//
//	return dst;
//
//}
//
//
//
//// Randbehandlung frechts
//
//Pixel LucyOMP::rightRim(double kernel, const Image& src, int y, int x, int width)
//
//{
//
//	double r = 0.0, g = 0.0, b = 0.0;
//
//	//Alle Pixel links normal hinzufuegen und Pixel rechts müssen mitweiss aufgefüllt werden
//
//#pragma omp parallel for
//
//	for (int w = x - 25; w <= x + 25; w++) {
//
//		if (x < width){
//
//			const Pixel &pixel = src[y][w];
//
//			r += pixel.r / kernel;
//
//			g += pixel.g / kernel;
//
//			b += pixel.b / kernel;
//
//		}
//
//		else{
//
//			r += 255.0 / kernel;
//
//			g += 255.0 / kernel;
//
//			b += 255.0 / kernel;
//
//		}
//
//	}
//
//	return Pixel(r, g, b);
//
//}
//
//
//
//
//
//// Wendet fuer jedes Pixel (Elementweise) die angewendete Operation op durch. Entweder (/) oder (*)
//
//Image LucyOMP::elementwiseArithmeticOperation(const Image &left, const Image &right, char op) {
//
//	// Dimension holen
//
//	const int height = left.height();
//
//	const int width = right.width();
//
//
//
//	double r = 0.0, g = 0.0, b = 0.0;
//
//	Image dst = Image(height, width);
//
//
//
//	for (int y = 0; y < height; y++) {
//
//		for (int x = 0; x < width; x++) {
//
//
//
//			const Pixel &gPixel = left[y][x];
//
//			const Pixel &bPixel = right[y][x];
//
//
//
//			if (op == '/') {
//
//				r = (gPixel.r / bPixel.r > 255) ? 255 : gPixel.r / bPixel.r;
//
//				g = (gPixel.g / bPixel.g > 255) ? 255 : gPixel.g / bPixel.g;
//
//				b = (gPixel.b / bPixel.b > 255) ? 255 : gPixel.b / bPixel.b;
//
//			}
//
//			// Es ist eine elementweise Multiplikation
//
//			else {
//
//				r = (gPixel.r * bPixel.r > 255) ? 255 : gPixel.r * bPixel.r;
//
//				g = (gPixel.g * bPixel.g > 255) ? 255 : gPixel.g * bPixel.g;
//
//				b = (gPixel.b * bPixel.b > 255) ? 255 : gPixel.b * bPixel.b;
//
//			}
//
//
//
//			dst[y][x] = Pixel(r, g, b);
//
//		}
//
//	}
//
//	return dst;
//
//}