#ifndef ZCONFIG_H
#define ZCONFIG_H

#include <zconfig_general.h>

#define ZBS_BEGIN namespace zbs {

#define ZBS_END }

// ****************** configuration file  for zrandomfile tests ************
//
//     this file uses as includes
//
//     /zbasesystem/zconfig_general.h      for general parameters
//     /zremote/zconfig_network.h          for network configuration
//
//

// Hack around stdlib bug with C++14.
//#include <initializer_list>  // force libstdc++ to include its config
//#undef _GLIBCXX_HAVE_GETS    // correct broken config
// End hack.





#define __DEBUG_LEVEL__ 4
#define __ZSA_DEBUG_LEVEL__ 1
#define __GENERIC_DEBUG_LEVEL__  3

#define __ERROR_MANAGEMENT_LEVEL__ 1


#define __ERROR__   1
#define __SEVERE__   2
#define __FATAL__    3

/** for implementing Thread management using ZThread objects :
 *
 *              - if defined with any value  : implements ZThread
 *              - thread types :
 *                  - ored with __ZTHREAD_POSIX__ implements POSIX threads management
 *                  - ored with __ZTHREAD_WINDOWS__ implements Windows threads management
 *              - threads management :
 *                  - ored with __ZTHREAD_AUTOMATIC__  makes zbs manage mutexes lock/unlock automatically
 */
#define __ZTHREAD_AUTOMATIC__           0x0100
//#define __FORCE_STD__

#ifdef __USE_WINDOWS__
#ifndef __FORCE_STD__
#define __USE_WIN_ZTHREAD__             0x0002
#define __USE_ZTHREAD__ (__USE_STD_ZTHREAD__ | __ZTHREAD_AUTOMATIC__)
#define __USE_STD_CHRONO__
#endif //__FORCE_STD__
#endif // __USE_WINDOWS__

#ifdef __USE_LINUX__
#ifndef __FORCE_STD__
#define __USE_POSIX_ZTHREAD__               0x0004
#define __USE_ZTHREAD__ (__USE_POSIX_ZTHREAD__ | __ZTHREAD_AUTOMATIC__)
#define __USE_STD_CHRONO__
#endif //__FORCE_STD__
#endif // __USE_LINUX__

#ifdef __FORCE_STD__
#define __USE_STD_ZTHREAD__               0x0008
#define __USE_ZTHREAD__ (__USE_STD_ZTHREAD__ | __ZTHREAD_AUTOMATIC__)
#define __USE_STD_CHRONO__
#endif //__FORCE_STD__







//#define __CRASH_ON_FATAL__  // for exiting with signal when an error occurs

#define __USE_ZRANDOMFILE__ 1


#define __USE_SELECT__

#include <znet/zconfig_network.h>


#endif // ZCONFIG_H
