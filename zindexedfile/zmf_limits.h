#ifndef ZMF_LIMITS_H
#define ZMF_LIMITS_H
#include <stddef.h>
#include <stdint.h>

#include <ztoolset/zlimit.h>

#define __ZMF_VERSION__ 2003001UL
#define __ZIF_VERSION__ 2003001UL

#define __ZRF_VERSION__  3002010UL
#define __ZRF_VERSION_CHAR__  "\"3.2-10\""
#define __ZRF_XMLVERSION_CONTROL__  "3.2-10"

#define __ZDIC_VERSION__ 30000UL
//#define __ZDIC_VERSION_CHAR__  "'3.0-00'"

const size_t _cst_zmf_journaling_allocation     = 20; //!< 20 operations in journaling before claiming for new allocation
const size_t _cst_zmf_journaling_reallocquota   = 10; //!< 10 operations block for space reallocation

const size_t _cst_zmf_descriptor_allocation     = 5 ;  //!< 5 indexes allocation space
const size_t _cst_zmf_descriptor_reallocquota   = 2 ;  //!< 2 indexes incremental number

const size_t _cst_zmf_history_allocation = 20 ;
const size_t _cst_zmf_history_reallocquota = 5 ;

typedef int64_t   zaddress_type;  //!< ZRandomFile address data type : nb : could be negative if unknown
const zaddress_type cst_HeaderOffset = 0L;

#endif // ZMF_LIMITS_H

