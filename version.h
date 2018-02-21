#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 0;
	static const long BUILD  = 81;
	static const long REVISION  = 452;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 201;
	#define RC_FILEVERSION 1,0,81,452
	#define RC_FILEVERSION_STRING "1, 0, 81, 452\0"
	static const char FULLVERSION_STRING [] = "1.0.81.452";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 81;
	

}
#endif //VERSION_H
