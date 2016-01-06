#ifndef GREENIFY_H
#define GREENIFY_H

#include "Algorithm.h"

class Greenify : public Algorithm
{
public:
   void process(const Parameters &params, const Image &src, Image &dst);
};

#endif
