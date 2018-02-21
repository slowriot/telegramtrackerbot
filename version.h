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
	static const long BUILD  = 46;
	static const long REVISION  = 254;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 166;
	#define RC_FILEVERSION 1,0,46,254
	#define RC_FILEVERSION_STRING "1, 0, 46, 254\0"
	static const char FULLVERSION_STRING [] = "1.0.46.254";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 46;
	

}
#endif //VERSION_H
