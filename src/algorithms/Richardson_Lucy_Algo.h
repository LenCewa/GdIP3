#ifndef RICHARDSON_LUCY_ALGO_H
#define RICHARDSON_LUCY_ALGO_H

#include "Algorithm.h"

class Richardson_Lucy_Algo : public Algorithm
{
public:
	void process(const Parameters &params, const Image &src, Image &dst);
	//Image richardsonLucyAlgo(Image &src, double kernel, int n);
	Image filter(double kernel, const Image &src);
	bool inRange(int x, const int width);
	Pixel kernelOperation(double kernel, const Image &src, int y, int x);
	Image elementwiseArithmeticOperation(const Image &left, const Image &right, char op);
};


#endif
