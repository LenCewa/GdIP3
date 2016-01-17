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
	Image bil = bilateralFilter(src);
	Image bin = binaryTraversion(src);
	dst = cartoonize(bil, bin);

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

double** CartoonizeOMP::create2DArray(int height, int width)
{
	double *row = (double*)calloc(height * width, sizeof(double));
	double **rowPt = (double**)calloc(height, sizeof(double*));

	for (int i = 0; i < height; i++)
		rowPt[i] = row + i * width;

	return rowPt;
}

void CartoonizeOMP::destroy2DArray(double **array)
{
	free(*array);
	free(array);
}

//------------------------------------
// BEGIN - BILATERAL
//------------------------------------

Image CartoonizeOMP::bilateralFilter(const Image &pic)
{
	const int H = pic.height();
	const int W = pic.width();
	Image result = Image(H, W);

	for (int y = 0; y < H; ++y)
#pragma omp parallel for
		for (int x = 0; x < W; ++x)
			if (!inRange(OMPkernelSize, y, x, H, W)) {
				result[y][x] = pic[y][x];
				continue;
			}
			else
				result[y][x] = bilateralKernel(pic, y, x);

	return result;
}

Pixel CartoonizeOMP::bilateralKernel(const Image &pic, int y, int x)
{
	// Hint: | y = i | x = j
	// Kernel Array initialisieren
	double **kernel = createAndInitializeKernel(pic, y, x);
	double zaehler_r = 0, zaehler_g = 0, zaehler_b = 0, nenner = 0;
	int h = 0; int w = 0;
	int appliedKernelS = OMPkernelSize / 2;

	for (int k = y - appliedKernelS; k < y + appliedKernelS; k++) {
#pragma omp parallel for
		for (int l = x - appliedKernelS; l < x + appliedKernelS; l++) {
			const Pixel &pixel = pic[k][l];
			zaehler_r += kernel[h][w] * pixel.r;
			zaehler_g += kernel[h][w] * pixel.g;
			zaehler_b += kernel[h][w] * pixel.b;
			nenner += kernel[h][w];
			w++;
		}
		w = 0; h++;
	}
	return Pixel(zaehler_r / nenner, zaehler_g / nenner, zaehler_b / nenner);
}

bool CartoonizeOMP::inRange(int ks, int y, int x, const int height, const int width)
{
	return ((y - ks / 2 < 0 || y + ks / 2 >= height) || (x - ks / 2 < 0 || x + ks / 2 >= width)) ? false : true;
}

double** CartoonizeOMP::createAndInitializeKernel(const Image &pic, int y, int x)
{
	// Hint: | y = i | x = j
	// Da pixel inRange sind
	int h = 0, w = 0;
	int appliedKernelS = OMPkernelSize / 2;
	// Kernel erstellen (double 2D Matrix)
	double **kernel = create2DArray(OMPkernelSize, OMPkernelSize);
	for (int k = y - appliedKernelS; k < y + appliedKernelS; k++) {
#pragma omp parallel for
		for (int l = x - appliedKernelS; l < x + appliedKernelS; l++) {
			kernel[h][w] = calculateKernelField(pic, y, x, k, l);
			w++;
		}
		w = 0; h++;
	}
	return kernel;
}

double CartoonizeOMP::calculateKernelField(const Image &pic, int y, int x, int k, int l) { return cDomain(y, x, k, l) * cEdge(pic, y, x, k, l); }

double CartoonizeOMP::cDomain(int y, int x, int k, int l)
{
	// Potenzregel benutzen und e^-(...) berechnen
	double summand1 = std::pow((y - k), 2);
	double summand2 = std::pow((x - l), 2);
	double dividend = 2 * std::pow(OMPsig_d, 2);
	double expression = -(summand1 + summand2) / dividend;
	return std::exp(expression);
}

double CartoonizeOMP::cEdge(const Image &pic, int y, int x, int k, int l)
{
	// Potenzregel benutzen und e^-(...) berechnen
	double r, g, b;// r_yx, g_yx, b_yx, r_kl, g_kl, b_kl;
	const Pixel &pixel_yx = pic[y][x]; // f(i,j)
	const Pixel &pixel_kl = pic[k][l]; // f(k,l)

	r = pixel_yx.r - pixel_kl.r;
	g = pixel_yx.g - pixel_kl.g;
	b = pixel_yx.b - pixel_kl.b;

	double zaehler = std::pow(euklidNorm(r, g, b), 2);
	double nenner = 2 * std::pow(OMPsig_r, 2);
	double expression = -(zaehler) / nenner;
	return std::exp(expression);
}

double CartoonizeOMP::euklidNorm(int r, int g, int b) { return std::sqrt((r * r) + (g * g) + (b * b)); }

//------------------------------------
// END - BILATERAL
//------------------------------------

//------------------------------------
// BEGIN - BINARY
//------------------------------------

Image CartoonizeOMP::binaryTraversion(const Image &pic)
{
	Image g = Image(pic.height(), pic.width());
	Image g_x = Image(pic.height(), pic.width());
	Image g_y = Image(pic.height(), pic.width());
	Image G = Image(pic.height(), pic.width());
	Image G_t = Image(pic.height(), pic.width());

	g = binaryFilter(pic, 'b'); // blur := 'b'
	g_x = binaryFilter(g, 'X');               // partielle Ableitung in X := 'dX'
	g_y = binaryFilter(g, 'Y');               // partielle Ableitung in Y := 'dY'
	G = edgeStrength(g_x, g_y);
	G_t = edgeDetection(G);

	return G_t;
}

Image CartoonizeOMP::binaryFilter(const Image &pic, char op)
{
	const int H = pic.height();
	const int W = pic.width();
	Image result = Image(H, W);

	for (int y = 0; y < H; ++y) {
#pragma omp parallel for
		for (int x = 0; x < W; ++x) {

			if (!inRange(3, y, x, H, W)) {
				result[y][x] = pic[y][x];
				continue;
			}
			else
				result[y][x] = binaryKernel(pic, y, x, op);
		}
	}
	return result;
}

Pixel CartoonizeOMP::binaryKernel(const Image &pic, int y, int x, char op)
{
	int h = 0; int w = 0;
	double r = 0.0, g = 0.0, b = 0.0;
	double **k;

	switch (op) {
	case 'b': k = getBlurMatrix();
		break;
	case 'X': k = getDerriveMatrix('x'); // Partielle Ableitung dX
		break;
	case 'Y': k = getDerriveMatrix('y'); // Partielle Ableitung dY
		break;
	default: printf("ERROR in binaryKernel switch-case. Wrong char input -> %c <-\n", op);
	}

	// Hint: wir sind inRange
	// Hint: Kernel size ist immer 3 --> (int) 3 / 2 = 1

	for (int i = y - 1; i <= y + 1; i++) {
#pragma omp parallel for
		for (int j = x - 1; j <= x + 1; j++) {
			const Pixel &pixel = pic[i][j];
			r += k[h][w] * pixel.r;
			g += k[h][w] * pixel.g;
			b += k[h][w] * pixel.b;
			w++;
		}
		w = 0; h++;
	}

	r = adaptPixelColor(r);
	g = adaptPixelColor(g);
	b = adaptPixelColor(b);

	return Pixel(r, g, b);
}

// Prüft ob ein Pixel im Intervall von [0;255] liegt
double CartoonizeOMP::adaptPixelColor(double rgb)
{
	if (rgb > 255.0)
		return 255.0;
	if (rgb < 0)
		return 0.0;
	return rgb;
}

double** CartoonizeOMP::getBlurMatrix()
{
	// Initialisiere blur Matrix 
	double **blur = create2DArray(3, 3);
	blur[0][0] = 1.0f / 16.0f; blur[0][1] = 2.0f / 16.0f; blur[0][2] = 1.0f / 16.0f;
	blur[1][0] = 2.0f / 16.0f; blur[1][1] = 4.0f / 16.0f; blur[1][2] = 2.0f / 16.0f;
	blur[2][0] = 1.0f / 16.0f; blur[2][1] = 2.0f / 16.0f; blur[2][2] = 1.0f / 16.0f;

	return blur;
}

double** CartoonizeOMP::getDerriveMatrix(char v)
{
	double **derrive = create2DArray(3, 3);
	if (v == 'x') {
		// Initialisiere
		derrive[0][0] = 1.0; derrive[0][1] = 0.0; derrive[0][2] = -1.0;
		derrive[1][0] = 2.0; derrive[1][1] = 0.0; derrive[1][2] = -2.0;
		derrive[2][0] = 1.0; derrive[2][1] = 0.0; derrive[2][2] = -1.0;
	}
	else {
		// Ableitung in Richtung 'y' Vektor (v) - Initialisiere:
		derrive[0][0] = 1.0; derrive[0][1] = 2.0; derrive[0][2] = 1.0;
		derrive[1][0] = 0.0; derrive[1][1] = 0.0; derrive[1][2] = 0.0;
		derrive[2][0] = -1.0; derrive[2][1] = -2.0; derrive[2][2] = -1.0;
	}
	return derrive;
}

Image CartoonizeOMP::edgeStrength(Image &picX, Image &picY)
{
	// Sollten gleich sein, aber man kann ja nie wissen... 
	const int H = std::min(picX.height(), picY.height());
	const int W = std::min(picX.width(), picY.width());
	Image result = Image(H, W);

	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x)
			result[y][x] = calcEdgeStrengthPixelwise(y, x, picX, picY);

	return result;
}

Pixel CartoonizeOMP::calcEdgeStrengthPixelwise(int y, int x, Image &picX, Image &picY)
{
	double r, g, b;
	const Pixel &px = picX[y][x];
	const Pixel &py = picY[y][x];

	r = std::sqrt(std::pow(px.r, 2) + std::pow(py.r, 2));
	g = std::sqrt(std::pow(px.g, 2) + std::pow(py.g, 2));
	b = std::sqrt(std::pow(px.b, 2) + std::pow(py.b, 2));

	r = adaptPixelColor(r);
	g = adaptPixelColor(g);
	b = adaptPixelColor(b);

	return Pixel(r, g, b);
}

Image CartoonizeOMP::edgeDetection(Image &pic)
{
	const int H = pic.height();
	const int W = pic.width();
	Image result = Image(H, W);

	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x)
			result[y][x] = calcEdgeDetectionPixelwise(y, x, pic);

	return result;
}

Pixel CartoonizeOMP::calcEdgeDetectionPixelwise(int y, int x, Image &pic)
{
	double r, g, b;
	const Pixel &pixel = pic[y][x];
	if (pixel.r > OMPtau && pixel.g > OMPtau && pixel.b > OMPtau){
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}
	else{
		r = 255.0;
		g = 255.0;
		b = 255.0;
	}
	/*r = (tau < pixel.r) ? 255.0 : 0.0;
	g = (tau < pixel.g) ? 255.0 : 0.0;
	b = (tau < pixel.b) ? 255.0 : 0.0;*/

	return Pixel(r, g, b);
}

//------------------------------------
// END - BINARY
//------------------------------------

//------------------------------------
// BEGIN - CARTOONIZED
//------------------------------------

Image CartoonizeOMP::cartoonize(Image &bil, Image &bin)
{
	// Sollten gleich sein, aber man kann ja nie wissen... 
	const int H = std::min(bil.height(), bil.height());
	const int W = std::min(bin.width(), bin.width());
	Image result = Image(H, W);

	for (int y = 0; y < H; ++y){
#pragma omp parallel for
		for (int x = 0; x < W; ++x) {
			const Pixel pBil = bil[y][x];
			const Pixel pBin = bin[y][x];
			result[y][x] = compare(pBil, pBin);
		}
	}
	return result;
}

// Vergleicht welcher der beiden Pixel genommen werden soll,
// da wir die schwarzen Kantenpixel benötigen
Pixel CartoonizeOMP::compare(const Pixel &pBil, const Pixel &pBin)
{
	double r, g, b;
	r = pBin.r;
	g = pBin.g;
	b = pBin.b;

	// Kante?
	if (r == 0.0 && g == 0.0 && b == 0.0)
		return Pixel(r, g, b); // schwarze Kante <=> Pixel(0.0, 0.0, 0.0)
	else
		return pBil;
}

//------------------------------------
// END - CARTOONIZED
//------------------------------------
