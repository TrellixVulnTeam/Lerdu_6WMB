/* $Id: bug00166.c 264838 2008-08-14 10:48:29Z tabe $ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int
main(int argc, char *argv[])
{
  gdImagePtr im;
  char path[1024];
  int c, result;

  sprintf(path, "%s/xpm/bug00166.xpm", GDTEST_TOP_DIR);
  im = gdImageCreateFromXpm(path);
  if (!im) {
    return 2;
  }
  c = gdImageGetPixel(im, 1, 1);
  if (gdImageRed(im, c)      == 0xAA
      && gdImageGreen(im, c) == 0xBB
      && gdImageBlue(im, c)  == 0xCC) {
    result = 0;
  } else {
    result = 1;
  }
  gdImageDestroy(im);
  return result;
}
