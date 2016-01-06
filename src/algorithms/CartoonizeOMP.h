#ifndef CARTOONIZEOMP_H
#define CARTOONIZEOMP_H

#include "Algorithm.h"

class CartoonizeOMP : public Algorithm
{
public:
	// Virtual (<=> Interface)
	void process(const Parameters &params, const Image &src, Image &dst);
};

#endif