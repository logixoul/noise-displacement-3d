#include <cmath>
//#include <algorithm>
#include <iostream>
//#include <vector>
//#include <map>
//#include <ctime>
//#include <cstdlib>
//#include <fstream>
//#include <strstream>
#include <string>
//#include <regex>
//#include <complex>
#include <cinder/app/AppBasic.h>
#include <cinder/gl/GlslProg.h>
#include <cinder/gl/Texture.h>
#include <cinder/gl/Fbo.h>
#include <cinder/gl/gl.h>
#include <cinder/ImageIo.h>
#include <cinder/Vector.h>
#include <cinder/Rand.h>
#include <boost/foreach.hpp>
#include <fftw3.h>
#include <cinder/Camera.h>
#include <boost/assign.hpp>
#include <cinder/params/Params.h> // not in cinder_lite

using namespace boost::assign;

#define foreach BOOST_FOREACH
using namespace ci;
using namespace std;
using namespace ci::app;
//using namespace boost::assign;
//using boost::irange;