#include <stdio.h>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"

int main(int argc, char **argv){
  IplImage *sourceImage = cvLoadImage("/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_B1.TIF",
      CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

  IplImage *destinationImage = cvCreateImage( cvGetSize( sourceImage ), IPL_DEPTH_8U, 3);

  cvSaveImage("test.tif", destinationImage);

  printf("Finish\n");

  cvReleaseImage( &sourceImage );
  cvReleaseImage( &destinationImage );

  return 0;

}

