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
	static const long BUILD  = 68;
	static const long REVISION  = 384;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 189;
	#define RC_FILEVERSION 1,0,68,384
	#define RC_FILEVERSION_STRING "1, 0, 68, 384\0"
	static const char FULLVERSION_STRING [] = "1.0.68.384";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 68;
	

}
#endif //VERSION_H
