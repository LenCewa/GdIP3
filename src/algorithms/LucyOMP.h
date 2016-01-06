#ifndef LUCYOMP_H
#define LUCYOMP_H

#include "Algorithm.h"

class LucyOMP : public Algorithm
{
public:
	// Virtual (<=> Interface)
	void process(const Parameters &params, const Image &src, Image &dst);
};

#endif