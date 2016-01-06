#ifndef CARTOONIZE_H
#define CARTOONIZE_H

#include "Algorithm.h"

class Cartoonize : public Algorithm
{
public:
	// Virtual (<=> Interface)
	void process(const Parameters &params, const Image &src, Image &dst);
	
	// "Hilfsmethode
	double** create2DArray(int height, int width);
	void destroy2DArray(double **array);
	
	// Bilateral
	Image bilateralFilter(const Image &pic);
	Pixel bilateralKernel(const Image &pic, int y, int x);
	bool inRange(int ks, int y, int x, const int height, const int width);
	double** createAndInitializeKernel(const Image &pic, int y, int x);
	double calculateKernelField(const Image &pic, int y, int x, int h, int w);
	double cDomain(int y, int x, int h, int w);
	double cEdge(const Image &pic, int y, int x, int h, int w);
	
	// Binary
	double euklidNorm(int r, int g, int b);
	Image binaryTraversion(const Image &pic);
	double** getBlurMatrix();
	double** getDerriveMatrix(char v);
	Image edgeStrength(Image &picX, Image &picY);
	Image edgeDetection(Image &pic);
	Pixel binaryKernel(const Image &pic, int y, int x, char op);
	Image binaryFilter(const Image &pic, char op);
	Pixel calcEdgeStrengthPixelwise(int y, int x, Image &picX, Image &picY);
	Pixel calcEdgeDetectionPixelwise(int y, int x, Image &pic);
	double adaptPixelColor(double rgb);

	// Cartoonize
	Image cartoonize(Image &bil, Image &bin);
	Pixel compare(const Pixel &pBil, const Pixel &pBin);
};

#endif