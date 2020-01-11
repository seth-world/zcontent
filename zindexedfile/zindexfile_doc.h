#ifndef ZINDEXFILE_DOC_H
#define ZINDEXFILE_DOC_H

// this file owns the doxygen general documentation for ZIndexFile

namespace zbs {

/**
 * @addtogroup ZMasterFileGroup
 * @{
 * @defgroup ZIndexGroup ZIndexFile : ZMasterFile Index management
 *
 * ZIndexFile allows to manage multiple index keys for a ZMasterFile.
 *
 *  It is possible to define as many indexes as necessary for a given ZMasterFile.
 *  Each key is independent, managed using ZIndexFile class.
 *
 *  For doing that, there are a set of methods @see ZIndexFileGroup
 *
 *  Key indexes definition is stored in a key dictionary
 *
 *
 * @}
*/


/**
@addtogroup ZIndexFileGroup
@{

@defgroup DictionaryGroup Key dictionary
Each field composing one key has to be defined and stored in a key dictionary.

CZKeyDictionary class is the objects that holds the key fields definitions, as a set of key data fields
@see DataField

This dictionary is part of ZIndexControlBlock object.
A ZIndexControlBlock holds the details of one index and is owned by ZMasterFile object.
ZIndexFile holds a pointer to ZIndexControlBlock it relates to within ZMasterFile object.


@defgroup DataFieldGroup Key data fields management
    Data fields type need to be recognized while aggregating them to make an index key because there may be some conversion to be done.
    Data field type is defined using ZType_type enum.
    @see ZType_type
    @see zgetZType()

    - Atomic data - i. e. unitary data (numeric data)  : Excepted for byte (int8_t or char - unsigned char), most systems use little endian internal representation.
        This memory representation of a multi-byte number reverses the order of bytes, so that putting it as is in a key will fool index searches and index creation.
        @see EndianConversion
    - Array : Array storage is considered as a collection of Atomic data fields, excepted for arrays of Char, considered as a fixed length CString.
    - Class : a Class field may be a class or a struct that will contain underneeth data fields. No distinction/serialization is made for underneeth data and user has to take into account the little endian & sign conversion by himself.
    - Pointer : Pointers are only allowed for temporary, query fields definition (when using seek facilities).
    @warning Pointers are not allowed for defining a key structure.
    @see ZIndexFile::zgetZType()

@image html ztype_type.jpg "ZType_type" width=10cm
@image latex ztype_type.jpg "ZType_type" width=10cm


@defgroup EndianConversion  Endianness imposes a special processing for key data.
    Endianness of numeric data imposes a special conversion while processing this kind of data for sorting it.

  The problem concerns Atomic data - i. e. unitary data (numeric data)  : Excepted for byte (int8_t or char - unsigned char), most systems use little endian internal representation.
  The memory representation of a multi-byte number reverses the order of bytes, so that putting it as is in a key will fool index searches and index creation.

  @note What is Endian ?
            - Big Endian : multi-byte numbers are stored left to right : from most significant to least significant. So this is a correct index representation: only sign must be treated (if signed data).
            - Little Endian : multi-byte numbers are stored in reverse byte order : from least significant to most significant (reverse order). This has to be reversed before creating or reading index key.

    In addition, sign management (minus sets the very most significant bit to 1) induces that a negative number looks greater than a positive number.

    These two elements (Endian and sign) will fool the index key search algorithms so that data has prior to be structured in a more logical way for sorting data.

Sign has to be neutralized within the number itself and a 'sign byte' is added with values :
   - 0 for negative number (so that any negative number is sorted before a positive number
   - 1 for positive number
        .

    Negative values are then complemented to their absolute value with 0xFF (as many bytes as the data size is long ) in order to get the effective value to be sorted by Key algorithms (and searched).

    ZIndexFile key content has its own data storage (called internal format).
    So that ZIndex Key content values are not 'human readable'.
    This is the reason, why there is an utility to print out ZIndex key content.
    @see ZMasterFile::zprintIndexFieldValues()    ZIndexFile::zprintKeyFieldsValues()



@defgroup ZIndexFileGroup ZIndexFile - Key Type management (definition, extraction & packing, depacking)
    ZIndexFile is the instance that manages one key for a ZMasterFile to which it relates.
    key definition is stored into a Key dictionary .
    It is a collection of ZIndexField_struct describing each field composing the key.

@see CZKeyDictionary
@see ZIndexField_struct
@see ZIndexFile
@see ZType_type

  @par  ZIndexFile Key definition
  A key may be composed of several fields, possibly not contiguous, of ZMasterFile record.
  Key fields definition is stored in the dictionary that contains any single field definition of the elements composing the index key.

  A dictionary is a collection of fields definition (ZIndexField_struct) dedicated to one and only one ZIndexFile.
 @see ZIndexField_struct
 @see ZKeyUtilities

  Each field definition contains its data type and information about the way its value is extracted from ZMasterFile record.

  @par What if a key field points out of a varying record boundaries ?

  As ZMasterFile has a varying record size, from case to case, a field may point to outside the MasterFile record highest boundary.
  In this case, the field is taken (as if it has been extracted) as a binary zeroed field, whatever its data type could be.
  In case of signed numeric arithmetic data type, this means 'negative zero' value, which is not a correct key data format (arithmetic zero is considered as positive).

  @par Using keys with ZMasterFiles

  Any ZIndexFile relates to a (father) ZMasterFile from which it gets its index data.
  A key structure must be defined using a Key dictionary and then will be populated transparently as soon as there will be an action done on ZMasterFile its relates to.

  Key is used to search ZMasterFile content using a key value, total value or partial value.

  @par Key Dictionary definition

  - Key dictionary creation : fields composing key are analyzed, validated and stored into Key dictionary CZKeyDictionary
  - Dictionary list : Key definition may be printed out.
        - Key-Insert Key insertion : extracts a key from a raw ZMasterFile record and generates a 'valid for index' key content. They key is inserted (or rejected if duplicate collision) into index file
        - Key-Search : key value must be composed, then used to search.
            - composing a key value :
            - searching for the composed key value
        ·

  @par Key registration for a ZMasterFile

  Once it has been defined, it is necessary to registrate this key within ZMasterFile.
  This can be done using ZMasterFile::CreateZIndexFile method.

   - The key definition is then stored within ZMasterFile header infra-data.
   - ZIndexFile files structure are created.
   - Index values are then built from ZMasterFile existing content.

  @par Key usage
    - key update from ZMaster file : Defined ZIndex keys are transparently added / removed / updated according the operation done on ZMasterFile.
        User does not have to take care of index, except in analyzing the returned status which could show key errors (the most probable one is ZS_DUPLICATEKEY that shows a duplicate key value on an index defined as ZST_NODUPLICATES)
    - search on key : user can search ZMasterFile content using key values per index, either total or partial key values.
    - remove from key : it is always possible to remove ZMasterFile content that matches key values (total or partial).
*/
/** @} */ //ZIndexFileGroup


} // namespace zbs

#endif // ZINDEXFILE_DOC_H

