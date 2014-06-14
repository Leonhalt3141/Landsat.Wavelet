
cmake_minimum_required (VERSION 2.6)

project( dspimg )
find_package( OpenCV REQUIRED )
add_executable( dspimg dspimg.cc ) 
target_link_libraries( dspimg ${OpenCV_LIBS} )
