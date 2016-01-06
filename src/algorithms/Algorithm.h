#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Parameters.h"
#include "Image.h"

#include <QObject>

class Algorithm : public QObject
{
Q_OBJECT

public:
   virtual ~Algorithm() {}
   virtual void process(const Parameters &params, const Image &src, Image &dst) = 0;
};

#endif
