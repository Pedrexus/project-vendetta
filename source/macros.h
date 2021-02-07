#pragma once

#ifdef _DEBUG
	#define noop (void)0;
#endif

#ifdef _DEBUG
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
// #define new new
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(x) if (x) delete[] x; x=NULL; 
#endif

