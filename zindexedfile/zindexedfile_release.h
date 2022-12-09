#ifndef ZINDEXEDFILE_RELEASE_H
#define ZINDEXEDFILE_RELEASE_H

/*

  full implementation of raw masterfile

    suppress usage of ZRecord

    change record surface : usage of URF fields

    usage of templates for accessing / feeding data

    key may be of varying length

    key size is no more used


    key is composed of fields with a special format :

    header
      1- ZType_type
      2- if ZType_type is not fixed length
         field length (URF_Varying_Size_type) in Raw format
      3- field data                           ---

    ZType_type fixed length (all types excepted varying strings)

      - All atomic types
      - ZDate
      - ZDate_full
      - Checksum
      - MD5


    ZType_type varying length
      - utf varying strings
      . utf fixed length string
      - ZDataBuffer (blob)

  ZBitset and ZBitsetFull are not eligible for key



  ZMasterfile

    key extraction : fields are extracted using URF parsing on record surface using field ranks given by key dictionary.




Test plan

  ZResource key

  utf8VaryingString key





 */

#endif // ZINDEXEDFILE_RELEASE_H
