#ifndef MIMAGE_H
#define MIMAGE_H

#include <vector>

struct Pixel
{
   double r;
   double g;
   double b;

   Pixel(double inR = 0.0, double inG = 0.0, double inB = 0.0)
      : r(inR), g(inG), b(inB)
   {}
};

typedef std::vector<Pixel > ImageRow;
typedef std::vector<ImageRow> ImageData;

class Image
{
public:
   Image(int height = 0, int width = 0);
   ~Image() {}

   inline int width() const { return (int) data[0].size(); }
   inline int height() const { return (int) data.size(); }

   inline ImageRow & operator[] (int y) { return data[y]; }
   inline const ImageRow & operator[] (int y) const { return data[y]; }

private:
   ImageData data;

};

#endif
