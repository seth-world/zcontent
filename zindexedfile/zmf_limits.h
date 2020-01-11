#ifndef ZMF_LIMITS_H
#define ZMF_LIMITS_H
#include <stddef.h>

const size_t _cst_zmf_journaling_allocation     = 20; //!< 20 operations in journaling before claiming for new allocation
const size_t _cst_zmf_journaling_reallocquota   = 10; //!< 10 operations block for space reallocation

const size_t _cst_zmf_descriptor_allocation     = 5 ;  //!< 5 indexes allocation space
const size_t _cst_zmf_descriptor_reallocquota   = 2 ;  //!< 2 indexes incremental number

const size_t _cst_zmf_history_allocation = 20 ;
const size_t _cst_zmf_history_reallocquota = 5 ;


#endif // ZMF_LIMITS_H

