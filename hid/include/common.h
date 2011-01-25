#ifndef _COMMON_H_
#define _COMMON_H_

#define countof(x)		(sizeof(x)/(sizeof((x)[0])))

#ifndef _WIN32
#define _snprintf snprintf
#endif

#define HIDCOMP_VERSION_MAJOR	    1
#define HIDCOMP_VERSION_MINOR	    5

#endif
