/*
 *Compile: g++ ReadGTIFF.cpp -lm -I/opt/local/include -lgeotiff -ltiff -L/opt/local/lib -o ReadGTIFF.o
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include<sstream> 
#include <algorithm>
#include <vector>

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

#define ANULL 0
#define PI 3.14159265359;

using namespace std;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

int Date2DOY(int Month, int Day){
    int DOY;
    if ( Month == 1 ) DOY = Day;
    if ( Month == 2 ) DOY = 31 + Day;
    if ( Month == 3 ) DOY = 59 + Day;
    if ( Month == 4 ) DOY = 90 + Day;
    if ( Month == 5 ) DOY = 120 + Day;
    if ( Month == 6 ) DOY = 151 + Day;
    if ( Month == 7 ) DOY = 181 + Day;
    if ( Month == 8 ) DOY = 212 + Day;
    if ( Month == 9 ) DOY = 243 + Day;
    if ( Month == 10 ) DOY = 273 + Day;
    if ( Month == 11 ) DOY = 303 + Day;
    if ( Month == 12 ) DOY = 333 + Day;
    return DOY;
}

struct GeoInfo {
    uint16 Depth;
    int ROWSPERSTRIP;
    double ResoX;
    double ResoY;
    double UpperLeft_X;
    double UpperLeft_Y;
    double UpperRight_X;
    double UpperRight_Y;
    double UpperLeft_X;
    double UpperLeft_Y;
    double LowerLeft_X;
    double LowerLeft_Y;
    double LowerRight_X;
    double LowerRight_Y;
} GeoInfo_t;

struct SRP_S {
    double LMIN_S;
    double LMAX_S;
    double QCALMIN_S;
    double QCALMAX_S;
    double SUN_AZI_S;
    double SUN_ELE_S;
    double SUN_DIS_S;
}SolarRadPara;

void GetMLTInfo(string filepath, int band_num){
    double LMIN, LMAX;
    double QCALMIN, QCALMAX;
    double SUN_AZI; SUN_ELE, SUN_DIS;
    
    int check;
    int dummy;
    dummy = 0;
    
    int DOY;
    int Month, Day;
    
    ostringstream sout;
    sout << band_num;
    string BNUM = sout.str();
    
    std::ifstream ifs(filepath);
    std::string str;
    
    ostringstream sout;
    sout << band_num;
    string BN = sout.str();
    
    std::string target;
    
    std::ifstream ifs(filepath);
    std::string str;
    
    if ( !ifs ){
        cout << "Error:Input data file not found" << endl;
        return EXIT_FAILURE;
    }
    
    while ( getline( ifs, str) ){
        target = "    RADIANCE_MAXIMUM_BAND_" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> LMAX;
        }
        target = "    RADIANCE_MINIMUM_BAND_" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1]), ' ');
            istringstream sin(subst[1]);
            sin >> LMIN;
        }
        target = "    QUANTIZE_CAL_MAX_BAND_" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> QCALMAX;
        }
        target = "    QUANTIZE_CAL_MIN_BAND_1" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> QCALMIN;
        }
        target = "    SUN_AZIMUTH";
        check = str.compare(0, 15, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> SUN_AZI;
        }
        target = "    SUN_ELEVATION";
        check = str.compare(0, 17, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> SUN_ELE;
        }
        target = "    DATE_ACQUIRED";
        check = str.compare(0, 17, target);
        if (check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            vector<string> subs = split(subst[1], '-');
            istringstream sin(subs[1]);
            sin >> Month;
            istringstream son(subs[2]);
            son >> Day;
            DOY = Date2DOY(Month, Day);
        }
        target = "    EARTH_SUN_DISTANCE";
        check = str.compare(0, 22, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1]);
            istringstream sin(subst[1]);
            sin >> SUN_DIS;
            dummy = 1;
        }
    }
    
    if ( dummy == 0 ){
        if ( DOY >= 1 && DOY < 15 ) SUN_DIS = 0.9832;
        if ( DOY >= 15 && DOY < 32 ) SUN_DIS = 0.9836;
        if ( DOY >= 32 && DOY < 46 ) SUN_DIS = 0.9853;
        if ( DOY >= 46 && DOY < 60 ) SUN_DIS = 0.9878;
        if ( DOY >= 60 && DOY < 74 ) SUN_DIS = 0.9909;
        if ( DOY >= 74 && DOY < 91 ) SUN_DIS = 0.9945;
        if ( DOY >= 91 && DOY < 106 ) SUN_DIS = 0.9993;
        if ( DOY >= 106 && DOY < 121 ) SUN_DIS = 1.0033;
        if ( DOY >= 121 && DOY < 135 ) SUN_DIS = 1.0076;
        if ( DOY >= 135 && DOY < 152 ) SUN_DIS = 1.0109;
        if ( DOY >= 152 && DOY < 166 ) SUN_DIS = 1.0140;
        if ( DOY >= 166 && DOY < 182 ) SUN_DIS = 1.0158;
        if ( DOY >= 182 && DOY < 196 ) SUN_DIS = 1.0167;
        if ( DOY >= 196 && DOY < 213 ) SUN_DIS = 1.0165;
        if ( DOY >= 213 && DOY < 227 ) SUN_DIS = 1.0149;
        if ( DOY >= 227 && DOY < 242 ) SUN_DIS = 1.0128;
        if ( DOY >= 242 && DOY < 258 ) SUN_DIS = 1.0092;
        if ( DOY >= 258 && DOY < 274 ) SUN_DIS = 1.0057;
        if ( DOY >= 274 && DOY < 288 ) SUN_DIS = 1.0011;
        if ( DOY >= 288 && DOY < 305 ) SUN_DIS = 0.9972;
        if ( DOY >= 305 && DOY < 319 ) SUN_DIS = 0.9925;
        if ( DOY >= 319 && DOY < 335 ) SUN_DIS = 0.9892;
        if ( DOY >= 335 && DOY < 349 ) SUN_DIS = 0.9860;
        if ( DOY >= 349 && DOY < 365 ) SUN_DIS = 0.9843;
        if ( DOY == 365 ) SUN_DIS = 0.9833;
        dummy = 2;
    }
    
    SolarRadPara.LMIN_S = LMIN;
    SolarRadPara.LMAX_S = LMAX;
    SolarRadPara.QCALMIN_S = QCALMIN;
    SolarRadPara.QCALMAX_S = QCALMAX;
    SolarRadPara.SUN_AZI_S = SUN_AZI;
    SolarRadPara.SUN_ELE_S = SUN_ELE;
    SolarRadPara.SUN_DIS_S = SUN_DIS;
    //return SolarRadPara;
}

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
    
    int width, height;
    int iCount;
    int RowsPerStrip;
    
    uint16 depth;
    
    if ( TIFFGetField( pTif, GTIFF_PIXELSCALE, &iCount, &pdReso) == 1 &&
         TIFFGetField( pTif, GTIFF_TIEPOINTS, &iCount, &pdTie ) == 1 &&
         TIFFGetField( pTif, TIFFTAG_BITSPERSAMPLE, &depth) == 1 &&
         TIFFGetField( pTif, TIFFTAG_IMAGEWIDTH, &width) == 1 &&
         TIFFGetField( pTif, TIFFTAG_IMAGELENGTH, &length) == 1 &&
         TIFFGetField( pTif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip, )) {
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
    
    GeoInfo_t.Depth = depth;
    GeoInfo_t.ROWSPERSTRIP = RowsPerStrip;
    GeoInfo_t.ResoX = dResoX;
    GeoInfo_t.ResoY = dResoY;
    GeoInfo_t.UpperLeft_X = UL_X;
    GeoInfo_t.UpperLeft_Y = UL_Y;
    GeoInfo_t.UpperRight_X = UR_X;
    GeoInfo_t.LowerLeft_X = LL_X;
    GeoInfo_t.LowerLeft_Y = LL_Y;
    GeoInfo_t.LowerRight_X = LR_X;
    GeoInfo_t.LowerRight_Y = LR_Y;
    
    //return GeoInfo_t;
}

void *ReadTiff(const char *filename ){
    TIFF *pTif = XTIFFOpen( filename, "r");
    return TIFF;
    XTIFFClose( pTif );
}

void ReadMLTfile(string filepath){
    double LMIN, LMAX;
    double QCALMIN, QCALMIN
    
    std::ifstream ifs( filepath );
    std::string str;
    
    if ( !ifs ){
        cout << "Error:Input data file not found" << endl;
        return EXIT_FAILURE;
    }
    
    while ( getline( ifs, str)){
        if (int compare(str, 27, ) )
    }
    cout >> hoge >> huga >> endl;
    
}

void *DN2Radiance(TIFF *pTif){
    int width, height;
    int row, col;
    
    double LMIN, LMAX;
    double QCALMIN, QCALMAX;
    
    SRP_S LandsatPara = GetMLTInfo(MLTfile);
    LMIN = LandsatPara.LMIN_S;
    LMAX = LandsatPara.LMAX_S;
    QCALMIN = LandsatPara.QCALMIN_S;
    QCALMAX = LandsatPara.QCALMAX_S;
    
    double *L; 
    uint8 *line;
    
    TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &height);
    
    line = (uint8*) _TIFFmalloc(TIFFScanlineSize(pTif));
    L = (double*) malloc(width * height * sizeof(double));
    
    for (row = 0; row < height; row++){
        TIFFReadScanline(pTif, line, row, 0);
        for (col = 0; col < width; col++ ){
            L[width * row + col] = ((LMAX - LMIN) / (QCALMAX - QCALMIN)) * (line[col] - QCALMIN) + LMIN; 
        }
    }
    return L;
}

void ReturnESUN(int Landsat_num, int band_num){
    if (Landsat_num == 5 ){
        if ( band_num == 1 ){
            return 1957.0;
        }
        if ( band_num == 2 ){
            return 1826.0;
        }
        if ( band_num == 3 ){
            return 1554.0;
        }
        if ( band_num == 4 ){
            return 1036.0;
        }
        if ( band_num == 5 ){
            return 215.0;
        }
        if ( band_num == 7 ){
            return 80.67;
        }
    }
}

void *Radiance2Reflectance(double *L, int Landsat_num, int band_num){
    int width, height;
    int row, col;
    
    double ESUN, d, Solar_Zenith;
    
    double *R;
    double *line;
    
    d = SolarRadPara.SUN_DIS_S;
    Solar_Zenith = ( 90 - SolarRadPara.SUN_ELE_S);
    ESUN = ReturnESUN(Landsat_num, band_num);
    
    TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &length);
    
    R = (double*) malloc(width * height * sizeof(double));
    
    for (row = 0; row < height; row++){
        TIFFReadScanline(pTif, line, row, 0);
        for(col = 0; col < width; col++){
            R[width * row + col] = PI * L[col] * d * d / ESUN / cos(Solar_Zenith);
        }
    }
    return R;
    
}

/*
void SetUpTiffDirectory(TIFF *pTif){
    
    int width, height;
    int iCount;
    
    width = TIFFGetField( pTif, TIFFTAG_IMAGEWIDTH, &width);
    height = TIFFGetField( pTif, TIFFTAG_IMAGELENGTH, &length);
    
    X = GeoInfo_t.UpperLeft_X + ( dResoX /2 );
    Y = GeoInfo_t.UpperLeft_Y - ( dResoY /2 );
    
    double tiepoints[8] = TIFFGetField( pTif, GTIFF_TIEPOINTS, &iCount, &pdTie);
    double pixscale[3] = {GeoInfo_t.ResoX, GeoInfo_t.ResoY, 0};
    
    
    TIFFSetFiled(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, GeoInfo_t.Depth);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, GeoInfo_t.ROWSPERSTRIP);
    
    TIFFSetField(tif, TIFFTAG_GEOTIEPOINTS, 8, tiepoints);
    TIFFSetField(tif, TIFFTAG_GEOPIXELSCALE, 3, pixscale);
}*/

void *WriteTiff(TIFF *tif, double R){
  
  int i;
  int height;
  
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &length);

  for(i=0; i< height; i++)
    if (!TIFFWriteScanline(tif, R, i, 0))
      TIFFError("WriteImage", "failure in WriteScanline\n");
}

void SetUpGeoKeys(string inpath, string outpath){
    int nCount;
    double *pdList = NULL;
    
    TIFF *intif = XTIFFOpen( filepath , "r" );
    TIFF *outtif = XTIFFOpen( filepath, "w" );
    
    if ( TIFFGetField( intif, GTIFF_TIEPOINTS, &nCount, &pdList) ){
        TIFFSetField( outtif, GTIFF_TIEPOINTS, nCount, pdList);
    }
    if ( TIFFGetField( intif, GTIFF_PIXELSCALE, &nCount, &pdList) ){
        TIFFSetField( outtif, GTIFF_PIXELSCALE, nCount, pdList);
    }
    if (TIFFGetField( intif, GTIFF_TRANSMATRIX, &nCount, &pdList) ){
        TIFFSetField( outtif, GTIFF_TRANSMATRIX, nCount, pdList);
    } 
    GTIF *gtif = GTIFNew( intif );
    
    gtif->gt_tif = outtif;
    gtif->gt_flags |= FLAG_FILE_MODIFIED;
    
    WriteTiff( outtif, R)
    
    GTIFWriteKeys( gtif );
    GTIFFree( gtif );
    
    XTIFFClose( gtif );
    
    XTIFFClose( intif );
    XTIFFClose( outtif );
}


int main(void){
    //TIFF *landsatimage;
    
    //landsatimage = ReadTiff("/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_B1.TIF");
    std::string filepath;
    filepath = "/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_B1.TIF";
    
    std::string outpath;
    outpath = "/Users/ken/workspace/Landsat/LT51070352000049HAJ00/test.TIF"
    
    std::int band_num;
    band_num = 1;
    std::int Landsat_num;
    Landsat_num = 5;
    
    double L;
    double R;
    
    GetMLTInfo(filepath, band_num);
    
    L = DN2Radiance(pTif);
    
    R = Radiance2Reflectance(L, Landsat_num, band_num);
    
    SetUpGeoKeys(filepath, outpath);
    
}
