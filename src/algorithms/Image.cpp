#include "Image.h"

Image::Image(int height, int width)
 : data(height)
{
   for (int y = 0; y < height; ++y) {
      data[y].resize(width);
      std::fill(data[y].begin(), data[y].end(), 0);
   }
}
