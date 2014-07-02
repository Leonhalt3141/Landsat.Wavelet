/*
 *Compile: g++ Calc.LandsatReflectance.cpp -lm -I/opt/local/include -lgeotiff -ltiff -L/opt/local/lib -o Calc.LandsatReflectance.o
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

#include "geotiff.h"
#include "geo_normalize.h"
#include "geo_tiffp.h"
#include "geo_keyp.h"
#include "xtiffio.h"
#include "cpl_serv.h"
#include "geovalues.h"

#define ANULL 0
#define PI 3.14159265359

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

void GetMLTInfo(const char *filepath, int band_num){
    double LMIN, LMAX;
    double QCALMIN, QCALMAX;
    double SUN_AZI, SUN_ELE, SUN_DIS;
    
    int check;
    int dummy;
    dummy = 0;
    
    int DOY;
    int Month, Day;
    
    
    std::ifstream ifs(filepath);
    std::string str;
    
    ostringstream sout;
    sout << band_num;
    string BN = sout.str();
    
    std::string target;
    
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
            vector<string> subst = split(substrs[1], ' ');
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
        target = "    QUANTIZE_CAL_MIN_BAND_" + BN;
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
            vector<string> subst = split(substrs[1], ' ');
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
}

void GetGeoInfo(TIFF *pTif){
    double UL_X, UL_Y, UR_X, UR_Y, LL_X, LL_Y, LR_X, LR_Y;
    double dResoX, dResoY;
    double *pdReso = NULL, *pdTie = NULL;
    
    int width, height;
    int iCount;
    int RowsPerStrip;
    
    uint16 depth;
    
    if ( TIFFGetField( pTif, GTIFF_PIXELSCALE, &iCount, &pdReso) == 1 &&
         TIFFGetField( pTif, GTIFF_TIEPOINTS, &iCount, &pdTie ) == 1 &&
         TIFFGetField( pTif, TIFFTAG_BITSPERSAMPLE, &depth) == 1 &&
         TIFFGetField( pTif, TIFFTAG_IMAGEWIDTH, &width) == 1 &&
         TIFFGetField( pTif, TIFFTAG_IMAGELENGTH, &height) == 1 &&
         TIFFGetField( pTif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip )) {
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
    
}

double *DN2Radiance(TIFF *pTif){
    int width, height;
    int row, col, X;
    
    double LMIN, LMAX;
    double QCALMIN, QCALMAX;
    
    //GetMLTInfo(MLTfile, band_num);
    LMIN = SolarRadPara.LMIN_S;
    LMAX = SolarRadPara.LMAX_S;
    QCALMIN = SolarRadPara.QCALMIN_S;
    QCALMAX = SolarRadPara.QCALMAX_S;
    
    double *L; 
    uint8 *line;
    
    TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &height);
    
    line = (uint8*) _TIFFmalloc(TIFFScanlineSize(pTif));
    L = (double*) malloc(width * height * sizeof(double));
    
    for (row = 0; row < height; row++){
        TIFFReadScanline(pTif, line, row);
        for (col = 0; col < width; col++ ){
            X = (width * row) + col;
            if(line[col] != ANULL){
                L[X] = ((LMAX - LMIN) / (QCALMAX - QCALMIN)) * ((double)line[col] - QCALMIN) + LMIN;
            } else if(line[col] == ANULL){
                L[X] = ANULL;
            }
        }
    }
    return L;
}

double ReturnESUN(int Landsat_num, int band_num){
    double ESUN;
    if (Landsat_num == 5 ){
        if ( band_num == 1 ){
            ESUN = 1957.0;
            return ESUN;
        }
        if ( band_num == 2 ){
            ESUN = 1826.0;
            return ESUN;
        }
        if ( band_num == 3 ){
            ESUN = 1554.0;
            return ESUN;
        }
        if ( band_num == 4 ){
            ESUN = 1036.0;
            return ESUN;
        }
        if ( band_num == 5 ){
            ESUN = 215.0;
            return ESUN;
        }
        if ( band_num == 7 ){
            ESUN = 80.67;
            return ESUN;
        }
    }
}

double *Radiance2Reflectance(TIFF *pTif, double *L, int Landsat_num, int band_num){
    int width, height;
    int row, col;
    
    int X;
    
    double ESUN, d, ELE, Solar_Zenith;
    
    double *R;
    double *line;
    
    line = (double*) _TIFFmalloc(TIFFScanlineSize(pTif));
    
    d = SolarRadPara.SUN_DIS_S;
    ELE = SolarRadPara.SUN_ELE_S;
    Solar_Zenith = ( 90.0 - ELE ) * PI / 180.0;
    ESUN = ReturnESUN(Landsat_num, band_num);
    
    TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &height);
    
    R = (double*) malloc(width * height * sizeof(double));
    
    for (row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            X = (width * row) + col;
            if(L[X] != ANULL){
                R[X] = (L[X] * d * d * PI) / (ESUN * cos(Solar_Zenith));
            } else if(L[X] == ANULL){
                R[X] = ANULL;
            }
        }
    }
    return R;
    
}


void SetUpTiffDirectory(TIFF *intif, TIFF *outtif){
    int width, height;
    int iCount;
    int nRowsPerSample;
    int nPhotometric;
    int nPlan;
    int depth;
    double *pdList = NULL;
    double *pdTie = NULL;
    double *pdReso = NULL;
    float xres, yres;
    short orientation;
    
    short tiepointsize, pixscalesize;
    double* tiepoints;
    double* pixscale;
    
    width = TIFFGetField( intif, TIFFTAG_IMAGEWIDTH, &width);
    height = TIFFGetField( intif, TIFFTAG_IMAGELENGTH, &height);
    
    /*double tiepoints = TIFFGetField( intif, GTIFF_TIEPOINTS, &iCount, &pdTie);
    double pixscale[3] = {GeoInfo_t.ResoX, GeoInfo_t.ResoY, 0};*/
    
    if ( TIFFGetField( intif, TIFFTAG_IMAGELENGTH, &height) ){
        TIFFSetField( outtif, TIFFTAG_IMAGELENGTH, height);
    }
    if ( TIFFGetField( intif, TIFFTAG_IMAGEWIDTH, &width)){
        TIFFSetField( outtif, TIFFTAG_IMAGEWIDTH, width);
    }
    if ( TIFFGetField( intif, TIFFTAG_ORIENTATION, &orientation)){
        TIFFSetField( outtif, TIFFTAG_ORIENTATION, orientation);
    }
    if ( TIFFGetField( intif, TIFFTAG_XRESOLUTION, &xres)){
        TIFFSetField( outtif, TIFFTAG_XRESOLUTION, xres);
    }
    if ( TIFFGetField( intif, TIFFTAG_YRESOLUTION, &yres)){
        TIFFSetField( outtif, TIFFTAG_YRESOLUTION, yres);
    }
    if ( TIFFGetField( intif, TIFFTAG_GEOTIEPOINTS, &tiepointsize, &tiepoints)){
        TIFFSetField( outtif, TIFFTAG_GEOTIEPOINTS, tiepointsize, tiepoints);
    }
    if ( TIFFGetField( intif, TIFFTAG_GEOPIXELSCALE, &pixscalesize, &pixscale)){
        TIFFSetField( outtif, TIFFTAG_GEOPIXELSCALE, pixscalesize, pixscale);
    }
    /*if ( TIFFGetField( intif, TIFFTAG_COMPRESSION, &pdList)){
        TIFFSetField( outtif, TIFFTAG_COMPRESSION, pdList);
    }*/
    if ( TIFFGetField(intif, TIFFTAG_PHOTOMETRIC, &nPhotometric)){
        TIFFSetField( outtif, TIFFTAG_PHOTOMETRIC, nPhotometric);
    }
    if ( TIFFGetField( intif, TIFFTAG_PLANARCONFIG, &nPlan)){
        TIFFSetField( outtif, TIFFTAG_PLANARCONFIG, nPlan);
    }
    if ( TIFFGetField( intif, TIFFTAG_BITSPERSAMPLE, &depth)){
        //TIFFSetField( outtif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
        TIFFSetField( outtif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        TIFFSetField( outtif, TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField( outtif, TIFFTAG_BITSPERSAMPLE, 64);
        TIFFSetField( outtif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
    }
    if (TIFFGetField( intif, TIFFTAG_ROWSPERSTRIP, &nRowsPerSample)){
        TIFFSetField( outtif, TIFFTAG_ROWSPERSTRIP, nRowsPerSample);
    }
    if (TIFFGetField( intif, TIFFTAG_GEOTIEPOINTS, &iCount, &pdTie)){
        TIFFSetField( outtif, TIFFTAG_GEOTIEPOINTS, iCount, pdTie);
    }
    if (TIFFGetField( intif, TIFFTAG_GEOPIXELSCALE, &iCount, &pdReso)){
        TIFFSetField( outtif, TIFFTAG_GEOPIXELSCALE, iCount, pdReso);
    }
}

void SetUpGeoKeys(GTIF *ingtif, GTIF *outgtif){
    geocode_t modelTypeCode, rasterModelType;
    
    geocode_t geoCDTypeCode, geodeticDatumCode, primeMeridiumCode, linearUnitCode, linearUnitValue, angularUnitCode, angularUnitValue, ellipsoideCode, semiMajorAxis, semiMinorAxis, geoInvertFlattening, angularUnitsCode, primeMeridianLongitude;
    
    
    if ( GTIFKeyGet( ingtif, GTModelTypeGeoKey, &modelTypeCode, 0, 1) ){
        GTIFKeySet( outgtif, GTModelTypeGeoKey, TYPE_SHORT, 1, modelTypeCode);
    }
    if ( GTIFKeyGet( ingtif, GTRasterTypeGeoKey, &rasterModelType, 0, 1) ){
        GTIFKeySet( outgtif, GTRasterTypeGeoKey, TYPE_SHORT, 1, rasterModelType);
    }
    if ( GTIFKeyGet( ingtif, GeographicTypeGeoKey, &geoCDTypeCode, 0, 1) ){
        GTIFKeySet( outgtif, GeographicTypeGeoKey, TYPE_SHORT, 1, geoCDTypeCode);
    }
    if ( GTIFKeyGet( ingtif, GeogGeodeticDatumGeoKey, &geodeticDatumCode, 0, 1)){
        GTIFKeySet( outgtif, GeogGeodeticDatumGeoKey, TYPE_SHORT, 1, geodeticDatumCode);
    }
    if ( GTIFKeyGet( ingtif, GeogPrimeMeridianGeoKey, &primeMeridiumCode, 0, 1)){
        GTIFKeySet( outgtif, GeogPrimeMeridianGeoKey, TYPE_SHORT, 1, primeMeridiumCode);
    }
    if ( GTIFKeyGet( ingtif, GeogLinearUnitsGeoKey, &linearUnitCode, 0, 1)){
        GTIFKeySet( outgtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1, linearUnitCode);
    }
    if ( GTIFKeyGet( ingtif, GeogLinearUnitSizeGeoKey, &linearUnitValue, 0, 1)){
        GTIFKeySet( outgtif, GeogLinearUnitSizeGeoKey, TYPE_SHORT, 1, linearUnitValue);
    }
    if ( GTIFKeyGet( ingtif, GeogAngularUnitsGeoKey, &angularUnitCode, 0, 1)){
        GTIFKeySet( outgtif, GeogAngularUnitsGeoKey, TYPE_SHORT, 1, angularUnitCode);
    }
    if ( GTIFKeyGet( ingtif, GeogAngularUnitSizeGeoKey, &angularUnitValue, 0, 1)){
        GTIFKeySet(outgtif, GeogAngularUnitSizeGeoKey, TYPE_SHORT, 1, angularUnitValue);
    }
    if ( GTIFKeyGet( ingtif, GeogEllipsoidGeoKey, &ellipsoideCode, 0, 1)){
        GTIFKeySet( outgtif, GeogEllipsoidGeoKey, TYPE_SHORT, 1, ellipsoideCode);
    }
    if ( GTIFKeyGet( ingtif, GeogSemiMajorAxisGeoKey, &semiMajorAxis, 0, 1)){
        GTIFKeySet( outgtif, GeogSemiMajorAxisGeoKey, TYPE_SHORT, 1, semiMajorAxis);
    }
    if ( GTIFKeyGet( ingtif, GeogSemiMinorAxisGeoKey, &semiMinorAxis, 0, 1)){
        GTIFKeySet( outgtif, GeogSemiMinorAxisGeoKey, TYPE_SHORT, 1, semiMinorAxis);
    }
    if ( GTIFKeyGet( ingtif, GeogInvFlatteningGeoKey, &geoInvertFlattening, 0, 1)){
        GTIFKeySet( outgtif, GeogInvFlatteningGeoKey, TYPE_SHORT, 1, geoInvertFlattening);
    }
    if ( GTIFKeyGet( ingtif, GeogAzimuthUnitsGeoKey, &angularUnitsCode, 0, 1)){
        GTIFKeySet( outgtif, GeogAzimuthUnitsGeoKey, TYPE_SHORT, 1, angularUnitsCode);
    }
    if ( GTIFKeyGet( ingtif, GeogPrimeMeridianLongGeoKey, &primeMeridianLongitude, 0, 1)){
        GTIFKeySet( outgtif, GeogPrimeMeridianLongGeoKey, TYPE_SHORT, 1, primeMeridianLongitude);
    }
    
      geocode_t projCSSystemCode, ProjCode, projCoordTransfCode, linearUnitsCode, linearUnitSize, projStdParallel1, projStdParallel2,projNatOriginLong,
	  projNatOriginLat, projFalseEasting, projFalseNorthing,
projFalseOriginLong, projFalseOriginLat, projFalseOriginEasting, projFalseOriginNorthing, projCenterLong, projCenterLat, projCenterEasting, projCenterNorthing, projScaleAtNatOrigin, projScaleAtCenter, projAzimuthAngle, projStraightVertPoleLong;
    
    
    // Projected CS Parameter Keys
    if ( GTIFKeyGet( ingtif, ProjectedCSTypeGeoKey, &projCSSystemCode, 0, 1)){
        GTIFKeySet( outgtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1, projCSSystemCode);
    }
    if ( GTIFKeyGet( ingtif, ProjectionGeoKey, &ProjCode, 0, 1)){
        GTIFKeySet( outgtif, ProjectionGeoKey, TYPE_SHORT, 1, ProjCode);
    }
    if ( GTIFKeyGet( ingtif, ProjCoordTransGeoKey,  &projCoordTransfCode, 0, 1)){
        GTIFKeySet( outgtif, ProjCoordTransGeoKey, TYPE_SHORT, 1, projCoordTransfCode);
    }
    if ( GTIFKeyGet( ingtif, ProjLinearUnitsGeoKey, &linearUnitsCode, 0, 1)){
        GTIFKeySet( outgtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1, linearUnitsCode);
    }
    if ( GTIFKeyGet( ingtif, ProjLinearUnitSizeGeoKey, &linearUnitSize, 0, 1)){
        GTIFKeySet( outgtif, ProjLinearUnitSizeGeoKey, TYPE_SHORT, 1, linearUnitSize);
    }
    if ( GTIFKeyGet( ingtif, ProjStdParallel1GeoKey, &projStdParallel1, 0, 1)){
        GTIFKeySet( outgtif, ProjStdParallel1GeoKey, TYPE_SHORT, 1, projStdParallel1);
    }
    if ( GTIFKeyGet( ingtif, ProjStdParallel2GeoKey, &projStdParallel2, 0, 1)){
        GTIFKeySet( outgtif, ProjStdParallel2GeoKey, TYPE_SHORT, 1, projStdParallel2);
    }
    if ( GTIFKeyGet( ingtif, ProjNatOriginLongGeoKey, &projNatOriginLong, 0, 1)){
        GTIFKeySet( outgtif, ProjNatOriginLongGeoKey, TYPE_SHORT, 1, projNatOriginLong);
    }
    if ( GTIFKeyGet( ingtif, ProjNatOriginLatGeoKey, &projNatOriginLat, 0, 1)){
        GTIFKeySet( outgtif, ProjNatOriginLatGeoKey, TYPE_SHORT, 1, projNatOriginLat);
    }
    if ( GTIFKeyGet( ingtif, ProjFalseEastingGeoKey, &projFalseEasting, 0, 1)){
        GTIFKeySet( outgtif, ProjFalseEastingGeoKey, TYPE_SHORT, 1, projFalseEasting);
    }
    if ( GTIFKeyGet( ingtif, ProjFalseNorthingGeoKey, &projFalseNorthing, 0, 1)){
        GTIFKeySet( outgtif, ProjFalseNorthingGeoKey, TYPE_SHORT, 1, projFalseNorthing);
    }
    if ( GTIFKeyGet( ingtif, ProjFalseOriginLongGeoKey, &projFalseOriginLong, 0, 1)){
        GTIFKeySet( outgtif, ProjFalseOriginLongGeoKey, TYPE_SHORT, 1, projFalseOriginLong);
    }
    if ( GTIFKeyGet( ingtif, ProjFalseOriginLatGeoKey, &projFalseOriginLat, 0, 1)){
        GTIFKeySet( outgtif, ProjFalseOriginLatGeoKey, TYPE_SHORT, 1, projFalseOriginLat);
    }
    if ( GTIFKeyGet( ingtif, ProjFalseOriginEastingGeoKey, &projFalseOriginEasting, 0, 1)){
        GTIFKeySet( outgtif, ProjFalseEastingGeoKey, TYPE_SHORT, 1, projFalseOriginEasting);
    }
    if ( GTIFKeyGet( ingtif, ProjFalseOriginNorthingGeoKey, &projFalseOriginNorthing, 0, 1)){
        GTIFKeySet( outgtif, ProjFalseNorthingGeoKey, TYPE_SHORT, 1, projFalseOriginNorthing);
    }
    if ( GTIFKeyGet( ingtif, ProjCenterLongGeoKey, &projCenterLong, 0, 1)){
        GTIFKeySet( outgtif, ProjCenterLongGeoKey, TYPE_SHORT, 1, projCenterLong);
    }
    if ( GTIFKeyGet( ingtif, ProjCenterLatGeoKey, &projCenterLat, 0, 1)){
        GTIFKeySet( outgtif, ProjCenterLatGeoKey, TYPE_SHORT, 1, projCenterLat);
    }
    if ( GTIFKeyGet( ingtif, ProjCenterEastingGeoKey, & projCenterEasting, 0, 1)){
        GTIFKeySet( outgtif, ProjCenterEastingGeoKey, TYPE_SHORT, 1, projCenterEasting);
    }
    if ( GTIFKeyGet( ingtif, ProjCenterNorthingGeoKey, &projCenterNorthing, 0, 1)){
        GTIFKeySet( outgtif, ProjCenterNorthingGeoKey, TYPE_SHORT, 1, projCenterNorthing);
    }
    if ( GTIFKeyGet( ingtif, ProjScaleAtNatOriginGeoKey, &projScaleAtNatOrigin, 0, 1)){
        GTIFKeySet( outgtif, ProjScaleAtNatOriginGeoKey, TYPE_SHORT, 1, projScaleAtNatOrigin);
    }
    if ( GTIFKeyGet( ingtif, ProjScaleAtCenterGeoKey, &projScaleAtCenter, 0,1)){
        GTIFKeySet( outgtif, ProjScaleAtNatOriginGeoKey, TYPE_SHORT, 1, projScaleAtCenter);
    }
    if ( GTIFKeyGet( ingtif, ProjAzimuthAngleGeoKey, &projAzimuthAngle, 0, 1)){
        GTIFKeySet( outgtif, ProjAzimuthAngleGeoKey, TYPE_SHORT, 1, projAzimuthAngle);
    }
    if ( GTIFKeyGet( ingtif, ProjStraightVertPoleLongGeoKey, &projStraightVertPoleLong, 0, 1)){
        GTIFKeySet( outgtif, ProjStraightVertPoleLongGeoKey, TYPE_SHORT, 1, ProjStraightVertPoleLongGeoKey);
    }
}

void *WriteTiff(TIFF *pTif, TIFF *oTif, double *R){
  
  int row, col, X;
  int height;
  int width;
  
  double *RLine;
  
  TIFFGetField(pTif, TIFFTAG_IMAGELENGTH, &height);
  TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
  RLine = (double*) malloc(width * sizeof(double));
  for(row=0; row < height; row++){
    for(col=0; col < width; col++){
        X = (width * row) + col;
        RLine[col] = R[X];
    }
    if (!TIFFWriteScanline(oTif, RLine, row, 0))
      TIFFError("WriteImage", "failure in WriteScanline\n");
  }
}

int main(void){
    
    const char *filepath = "/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_B1.TIF";
    
    const char *mltfilepath = "/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_MTL.txt";
    
    const char *outpath = "/Users/ken/workspace/Landsat/LT51070352000049HAJ00/test.TIF";
    
    int band_num;
    band_num = 1;
    int Landsat_num;
    Landsat_num = 5;
    
    double *L;
    double *R;
    
    TIFF *pTif, *oTif;
    GTIF *pGTif, *oGTif;
    
    GetMLTInfo(mltfilepath, band_num);
    
    pTif = XTIFFOpen(filepath, "r");
    oTif = XTIFFOpen(outpath, "w");
    
    pGTif = GTIFNew( pTif );
    oGTif = GTIFNew( oTif );
    L = DN2Radiance(pTif);
    R = Radiance2Reflectance(pTif, L, Landsat_num, band_num);
    SetUpTiffDirectory(pTif, oTif);
    SetUpGeoKeys(pGTif, oGTif);
    WriteTiff(pTif, oTif, R);
    GTIFWriteKeys(oGTif);
    GTIFFree(pGTif);
    GTIFFree(oGTif);
    XTIFFClose(pTif);
    XTIFFClose(oTif);
    
}
