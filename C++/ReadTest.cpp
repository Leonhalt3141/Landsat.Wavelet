/*
 * Compile: g++ ReadTest.cpp -o ReadTest.o
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream> 
#include <algorithm>
#include <iterator>
#include <vector>

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

struct SRP_S {
    double LMIN_S;
    double LMAX_S;
    double QCALMIN_S;
    double QCALMAX_S;
    double SUN_ELE_S;
    double SUN_DIS_S;
}SolarRadPara;

void test(){
    
    double LMIN, LMAX;
    double QCALMIN, QCALMAX;
    double SUN_ELE, SUN_DIS;
    
    int check;
    int dummy;
    dummy = 0;
    
    int DOY;
    
    int band_num;
    band_num = 1;
    
    int Month, Day;
    
    ostringstream sout;
    sout << band_num;
    string BN = sout.str();
    
    std::string target;
    
    std::ifstream ifs("/Users/ken/workspace/Landsat/LT51070352000049HAJ00/LT51070352000049HAJ00_MTL.txt");
    std::string str;
    
    /*if( !ifs ){
        cout << "Error:Input data file not found" << endl;
        return EXIT_FAILURE;
    }*/
    
    while ( getline( ifs, str)){
        target = "    RADIANCE_MAXIMUM_BAND_" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> LMAX;
            //std::cout << "[" << LMAX << "]" << std::endl;
        }
        target = "    RADIANCE_MINIMUM_BAND_" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> LMIN;
            //std::cout << "[" << LMIN << "]" << std::endl;
        }
        target = "    QUANTIZE_CAL_MAX_BAND_" + BN;
        check = str.compare(0, 27, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> QCALMAX;
            //std::cout << "[" << QCALMAX << "]" << std::endl;
        }
        target = "    QUANTIZE_CAL_MIN_BAND_" + BN;
        check = str.compare(0, 27, target);
        if( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> QCALMIN;
            //std::cout << "[" << QCALMIN << "]" << std::endl;
        }
        target = "    SUN_ELEVATION";
        check = str.compare(0, 17, target);
        if ( check == 0 ) {
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            istringstream sin(subst[1]);
            sin >> SUN_ELE;
        }
        target = "    DATE_ACQUIRED";
        check = str.compare(0, 17, target);
        if ( check == 0 ){
            vector<string> substrs = split(str, '=');
            vector<string> subst = split(substrs[1], ' ');
            vector<string> subs = split(subst[1], '-');
            istringstream sin(subs[1]);
            sin >> Month;
            istringstream son(subs[2]);
            son >> Day;
            DOY = Date2DOY(Month, Day);
            //std::cout << "DOY: " << DOY << std::endl;
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
    
    //SRParameters_t SolarRadPara;
    
    SolarRadPara.LMIN_S = LMIN;
    SolarRadPara.LMAX_S = LMAX;
    SolarRadPara.QCALMIN_S = QCALMIN;
    SolarRadPara.QCALMAX_S = QCALMAX;
    SolarRadPara.SUN_ELE_S = SUN_ELE;
    SolarRadPara.SUN_DIS_S = SUN_DIS;
    /*
    //SolarRadPara = { LMIN, LMAX, QCALMIN, QCALMAX };
    std::cout << SolarRadPara.LMIN_S << std::endl;
    std::cout << SolarRadPara.LMAX_S << std::endl;
    std::cout << SolarRadPara.QCALMIN_S << std::endl;
    std::cout << SolarRadPara.QCALMAX_S << std::endl;
    std::cout << SolarRadPara.SUN_ELE_S << std::endl;
    std::cout << SolarRadPara.SUN_DIS_S << std::endl;*/
}

int main(){
    test();
    std::cout << SolarRadPara.LMIN_S << std::endl;
    std::cout << SolarRadPara.SUN_DIS_S << std::endl;
}