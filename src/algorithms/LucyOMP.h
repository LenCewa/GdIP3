#ifndef LUCYOMP_H
#define LUCYOMP_H

#include "Algorithm.h"

class LucyOMP : public Algorithm
{
public:
	// Virtual (<=> Interface)
	void process(const Parameters &params, const Image &src, Image &dst);

	Image filter(double kernel, const Image &src);
	bool inRange(int x, const int width);
	Pixel kernelOperation(double kernel, const Image &src, int y, int x);
	Image elementwiseArithmeticOperation(const Image &left, const Image &right, char op);
	Pixel rightRim(double kernel, const Image& src, int y, int x, int width);
};

#endif