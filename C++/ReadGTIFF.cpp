/*
 *Compile: g++ ReadGTIFF.cpp -I/opt/local/include -lgeotiff -ltiff -L/opt/local/lib -o ReadGTIFF.o
 */
 
#include <stdio.h>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "geotiff.h"
#include "geo_normalize.h"
#include "geo_tiffp.h"
#include "geo_keyp.h"
#include "xtiffio.h"
#include "cpl_serv.h"
#include "geovalues.h"

void *ScanTiff(TIFF *pTif){
    int width, height;
    int row, col;
    
    uint8 *buf, *line;
    
    TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &height);
    
    line = (unit8*) _TIFFmalloc(TIFFScanlineSize(pTif));
    buf = (uint8*) malloc(width * height * sizeof(uint8));
    
    for (row = 0; row < height; row++){
        TIFFReadScanline(pTif, line, row, 0);
        for (col = 0; col < width; col++){
            buf[width * row + col] = line[col];
        }
    }
    return buf;
    free(buf);
    _TIFFfree(line);
}

void GetGeoInfo(TIFF *pTif){
    double UL_X, UL_Y, UR_X, UR_Y, LL_X, LL_Y, LR_X, LR_Y;
    double ResoX, ResoY;
    double *pdReso = NULL, *pdTile = NULL;
    
    int iCount;
    
    uint16 depth;
    
    if ( TIFFGetField( pTif, GTIFF_PIXELSCALE, &iCount, &pdReso) == 1 &&
         TIFFGetField( pTif, GTIFF_TIEPOINTS, &iCount, &pdTie ) == 1 &&
         TIFFGetField( pTif, TIFFTAG_BITSPERSAMPLE, &depth) == 1 &&
         TIFFGetField( pTif, TIFFTAG_IMAGEWIDTH, &width) == 1 &&
         TIFFGetField( pTif, TIFFTAG_IMAGELENGTH, &length) == 1 ) {
             dResoX = pdReso[0];
             dResoY = pdReso[1];
             UL_X = pdTie[3] - ( dResoX /2 );
             UL_Y = pdTie[4] + ( dResoY / 2);
             UR_X = UL_X + ( dResoX * width );
             UR_Y = UL_Y;
             LL_X = UL_X;
             LL_Y = UL_Y - ( dResoY * height );
             LR_X = UR_X;
             LR_Y = LL_Y;
         }
    
    typedef struct {
        double ResoX;
        double ResoY;
        double UpperLeft_X;
        double UpperLeft_Y;
        double UpperRight_X;
        double UpperRight_Y;
        double LowerLeft_X;
        double LowerLeft_Y;
        double LowerRight_X;
        double LowerRight_Y;
    } GeoInfo_t;
    
    GeoInfo_t geoinf;
    
    geoinf = { dResoX, dResoY, UL_X, UL_Y, UR_X, UR_Y, LL_X, LL_Y,
        LR_X, LR_Y };
    return GeoInfo_t;
}

void *ReadTiff(const char *filename ){
    TIFF *pTif = XTIFFOpen( filename, "r");
    return TIFF;
}


void *ReadTiff(const char *filename ){
  double dTL_X, dTL_Y, dTR_X, dTR_Y, dBL_X, dBL_Y, dBR_X, dBR_Y;
  double dResoX, dResoY;
  double *pdReso = NULL, *pdTie = NULL;
  
  int iCount;
  int width;
  int height;
  int row, col;
  
  uint16 depth;
  
  
  void *nEllipse;
  
  int N;
  
  //tdata_t buf;
  uint8 *buf, *line;
  
  GTIF *pGTif;
  TIFF *pTif = XTIFFOpen( filename, "r");
  
  
  if( TIFFGetField( pTif, GTIFF_PIXELSCALE, &iCount, &pdReso ) == 1 &&
      TIFFGetField( pTif, GTIFF_TIEPOINTS, &iCount, &pdTie ) == 1 &&
      TIFFGetField( pTif, TIFFTAG_BITSPERSAMPLE, &depth ) == 1 &&
      TIFFGetField( pTif, TIFFTAG_IMAGEWIDTH, &width) == 1 &&
      TIFFGetField( pTif, TIFFTAG_IMAGELENGTH, &height) == 1 ) {
    
    dResoX = pdReso[0];
    dResoY = pdReso[1];
    dTL_X = pdTie[3] - ( dResoX / 2 );
    dTL_Y = pdTie[4] + ( dResoY / 2 );
    dTR_X = dTL_X + ( dResoX * width ); 
    dTR_Y = dTL_Y;
    dBL_X = dTL_X;
    dBL_Y = dTL_Y - ( dResoY * height );
    dBR_X = dTR_X;
    dBR_Y = dBL_Y;
  }
  pGTif = GTIFNew( pTif );
  
  GTIFKeyGet( pGTif, GeogEllipsoidGeoKey, &nEllipse, 0, 1 );
  
  printf("Upper Left:\t%lf\t%f \nUpper Right:\t%lf\t%lf\nLower Left:\t%lf\t%lf\nLower Right:\t%lf\t%lf\nbit: %d\nWidth: %d\tHeight: %d\n", 
            dTL_X, dTL_Y,
            dTR_X, dTR_Y,
            dBL_X, dBL_Y,
            dBR_X, dBR_Y,
            depth,
            width, height);
  
  line = ( uint8 * ) _TIFFmalloc(TIFFScanlineSize(pTif));
  buf = ( uint8 * ) malloc(width * height * sizeof( uint8 ));
  
  for (row = 0; row < height; row++){
    TIFFReadScanline(pTif, line, row, 0);
    printf("%d: ", row);
    for (col = 0; col < width; col++ ){
        buf[width * row + col] = line[col];
        //printf("%d ", line[col]);
        //_TIFFfree(buf);
    }
    for (col = 0; col < width; col = col + (width / 8) - 1){
        printf("%d ", buf[width * row + col]);
    }
    printf("\n");
    //TIFFReadScanline(pTif, buf, row);
  }
  free(buf);
  _TIFFfree(line);
  
  return pGTif;
  
  GTIFFree( pGTif );

  XTIFFClose( pTif );
}

/*
void SetUpTiffDirectory(TIFF *tif){
    double tiepoints[6] = {0, 0, 0, 130.0, 32.0,0.0};
    double pixscale[3] = {1,1,0};
    
    TIFFSetFiled(tif, TIFFTAG_IMAGEWIDTH, WIDTH);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, HEIGHT);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 20L);
    
    TIFFSetField(tif, TIFFTAG_GEOTIEPOINTS, 6m tiepoints);
    TIFFSetField(tif, TIFFTAG_GEOPIXELSCALE, 3, pixscale);
}*/

/*
void SetUpGeoKeys(GTIF *gtif){
  GTIFKeySet(gtif, GTModelTypeGeoKey, TYPE_SHORT, 1, ModelGeographic);
  GTIFKeySet(gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1, RasterPixelIsArea);
  GTIFKeySet(gtif, GTCitationGeoKey, TYPE_ASCII, 0, "Just An Example");
  GTIFKeySet(gtif, GeographicTypeGeoKey, TYPE_SHORT, 1, KvUserDefined);
  GTIFKeySet(gtif, GeogCitationGeoKey, TYPE_ASCII, 0, "Everest Ellipsoid Used.");
  GTIFKeySet(gtif, GeogAngularUnitsGeoKey, TYPE_SHORT, 1, Angular_Degree);
  GTIFKeySet(gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1, Linear_Meter);
  GTIFKeySet(gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT, 1, KvUserDefined);
  GTIFKeySet(gtif, GeogEllipsoidGeoKey, TYPE_SHORT, 1, Ellipse_Everest_1830_1967_Definition);
  GTIFKeySet(gtif, GeogSemiMajorAxisGeoKey, TYPE_DOUBLE, 1, (double)6377298.556);
  GTIFKeySet(gtif, GeogInvFlatteningGeoKey, TYPE_DOUBLE, 1, (double)300.8017);
}*/

void *WriteTiff(TIFF *tif, long WIDTH, long HEIGHT){
  int i;
  char buffer[WIDTH];
  memset(buffer, 0, (size_t)WIDTH);
  for(i=0; i<HEIGHT; i++)
    if (!TIFFWriteScanline(tif, buffer, i, 0))
      TIFFError("WriteImage", "failure in WriteScanline\n");
}

int main(void){
  ReadTiff("/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_B1.TIF");
}
