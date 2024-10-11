/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "zcontent", "index.html", [
    [ "Introduction", "index.html#MainIntroduction", null ],
    [ "What content management are we talking about ?", "index.html#MainWhat", [
      [ "Huge contents management with ZRandomFile", "index.html#MainContent", null ],
      [ "Index key search with ZMasterFile", "index.html#MainIndex", null ],
      [ "Statistical and dump tools", "index.html#MainStat", null ],
      [ "Maintenance and repair tools", "index.html#MainRepair", null ]
    ] ],
    [ "Software architecture", "index.html#MainArchitecture", [
      [ "zbasesystem toolset", "index.html#MainZBSToolset", null ],
      [ "ZRandomFile Architecture", "index.html#MainZRFArch", null ],
      [ "ZMasterFile Architecture", "index.html#MainZMFArch", null ],
      [ "ZICM Locks management and usage", "index.html#MainLock", null ]
    ] ],
    [ "To Do list for ZRandomFile and ZMasterFile ZIndexFile", "index.html#MainZRFZMFToDoList", [
      [ "ZRandomFile Todo list", "index.html#MainZRFToDoList", null ],
      [ "ZRandomFile Todo list", "index.html#MainZMFToDoList", null ]
    ] ],
    [ "Tests plans", "index.html#MainTest", [
      [ "ZRandomFile Tests plan", "index.html#MainZRFTestPlan", null ],
      [ "ZMasterFile Tests plan", "index.html#MainZMFTestPlan", null ]
    ] ],
    [ "ZMasterFile Reference Documentation", "_z_master_file_page.html", [
      [ "ZMasterFile general presentation", "_z_master_file_page.html#ZMFMainZMFGroup", null ],
      [ "Accessing ZMasterFile and its indexes", "_z_master_file_page.html#ZMFMainZMasterFileAccess", null ],
      [ "ZIndexFile : ZMasterFile Index management", "_z_master_file_page.html#ZMFMainZIndex", [
        [ "ZIndexFile - Key Type management (definition, extraction & packing, depacking)", "_z_master_file_page.html#ZMFMainZIndexFileGroup", null ],
        [ "Key dictionary", "_z_master_file_page.html#ZMFMainZDictionnary", null ],
        [ "Key data fields management", "_z_master_file_page.html#ZMFMainDataFieldGroup", null ]
      ] ],
      [ "Managing ZIndex Key data types", "_z_master_file_page.html#ZMFMainZIdxType", [
        [ "Querying ZMasterFile using ZKey", "_z_master_file_page.html#ZMFMainZKeyGroup", null ]
      ] ],
      [ "ZMasterFile Performance statistics", "_z_master_file_page.html#ZMFMainZMFSTATS", null ],
      [ "Physical layer", "_z_master_file_page.html#ZMFMainZMFPhysicalZMasterFile", null ],
      [ "ZMasterFile Utilities", "_z_master_file_page.html#ZMFMainUtil", null ],
      [ "Changing Files parameters and indexes", "_z_master_file_page.html#ZMFMainCHG", null ]
    ] ],
    [ "Starting up using ZMasterFile", "_z_m_f_quick_start.html", [
      [ "Introduction - Example presentation", "_z_m_f_quick_start.html#ZMFQSIntro", [
        [ "Required include files", "_z_m_f_quick_start.html#ZMFQSRequired", null ],
        [ "Data definitions", "_z_m_f_quick_start.html#ZMFQSDataDef", null ]
      ] ],
      [ "Instantiating ZMasterFile", "_z_m_f_quick_start.html#ZMFQSInstantiate", [
        [ "version 1", "_z_m_f_quick_start.html#ZMFQSInsV1", null ],
        [ "version 2", "_z_m_f_quick_start.html#ZMFQSInsV2", null ]
      ] ],
      [ "Creating a ZMasterFile", "_z_m_f_quick_start.html#ZMFQSCreating", null ],
      [ "Setting ZMasterFile options", "_z_m_f_quick_start.html#ZMFQSSetting", null ],
      [ "Creating Indexes", "_z_m_f_quick_start.html#ZMFQSCreatIdx", [
        [ "First index", "_z_m_f_quick_start.html#ZMFQSCreatIdxFirst", null ],
        [ "Second index", "_z_m_f_quick_start.html#ZMFQSCreatIdxSecond", null ],
        [ "Getting file's infra-data in XML format", "_z_m_f_quick_start.html#ZMFQSGetXML", null ]
      ] ],
      [ "Setting indexes option", "_z_m_f_quick_start.html#ZMFQSSetIdx", null ],
      [ "Opening and closing ZMasterFile", "_z_m_f_quick_start.html#ZMFQSOpening", null ],
      [ "Feeding ZMasterFile", "_z_m_f_quick_start.html#ZMFQSFeeding", null ],
      [ "Adding indexes once file has been populated", "_z_m_f_quick_start.html#ZMFQSAddIdx", null ],
      [ "Modifying Indexes", "_z_m_f_quick_start.html#ZMFQSModIdx", null ],
      [ "Searching for content with a ZMasterFile", "_z_m_f_quick_start.html#ZMFQSSearching", [
        [ "Index searches and free sequential searches", "_z_m_f_quick_start.html#ZMFQSSearchTypes", null ],
        [ "Access ZMasterFile using its indexes.", "_z_m_f_quick_start.html#ZMFQSIndexAccess", null ],
        [ "Exact key value or partial key value in a search operation", "_z_m_f_quick_start.html#ZMFQSPartialKey", null ],
        [ "Using ZKey object", "_z_m_f_quick_start.html#ZMFQSZKey", null ],
        [ "Using collection of records", "_z_m_f_quick_start.html#ZMFQSCollection", null ],
        [ "Searching index with a range of key values - or Interval search", "_z_m_f_quick_start.html#ZMFQSInterval", null ]
      ] ],
      [ "Removing content from ZMasterFile", "_z_m_f_quick_start.html#ZMFQSRemoving", [
        [ "Removing a single record", "_z_m_f_quick_start.html#ZMFQSRemoveSingle", null ],
        [ "Removing a set (collection) of records", "_z_m_f_quick_start.html#ZMFQSRemoveMass", null ]
      ] ],
      [ "Getting statistical data", "_z_m_f_quick_start.html#ZMFQSGetStat", null ],
      [ "Converting an existing ZRandomFile to a ZMasterFile.", "_z_m_f_quick_start.html#ZMFQSConverting", null ],
      [ "Making some maintenance operations on file", "_z_m_f_quick_start.html#ZMFQSMaintenance", null ]
    ] ],
    [ "ZRandomFile Reference Documentation", "_z_random_file_page.html", [
      [ "ZRandomFile : Random Access File for content storage", "_z_random_file_page.html#ZRFMainZRandomFileGroup", null ],
      [ "ZRandomFile architecture", "_z_random_file_page.html#ZRFMainArchitecture", [
        [ "ZRandomFile Physical layer", "_z_random_file_page.html#ZRFMainPhysical", null ],
        [ "ZRandomFile Blocks Pools", "_z_random_file_page.html#ZRFMainPools", null ],
        [ "ZRandomFile file header structure", "_z_random_file_page.html#ZRFMainHeader", null ]
      ] ],
      [ "Using adhoc fields with ZRandomFile", "_z_random_file_page.html#ZRFMainFree", null ],
      [ "Searching in free sequential mode", "_z_random_file_page.html#ZRFMainFreeSearch", null ]
    ] ],
    [ "Starting up using ZRandomFile", "_z_r_f_quick_start.html", [
      [ "Introduction - Example presentation", "_z_r_f_quick_start.html#ZRFQSIntro", [
        [ "Required include files", "_z_r_f_quick_start.html#ZRFQSRequired", null ],
        [ "Data definitions", "_z_r_f_quick_start.html#ZRFQSDataDef", null ]
      ] ],
      [ "Instantiating ZRandomFile", "_z_r_f_quick_start.html#ZRFQSInstantiate", [
        [ "version 1", "_z_r_f_quick_start.html#ZRFQSInsV1", null ],
        [ "version 2", "_z_r_f_quick_start.html#ZRFQSInsV2", null ]
      ] ],
      [ "Creating a ZRandomFile", "_z_r_f_quick_start.html#ZRFQSCreating", null ],
      [ "Setting ZRandomFile options", "_z_r_f_quick_start.html#ZRFQSSetting", [
        [ "Setting some options and openning the file", "_z_r_f_quick_start.html#ZRFQSSetParams", null ],
        [ "Loading documents and meta data into ZRandomFile", "_z_r_f_quick_start.html#ZRFQSLoad", null ]
      ] ],
      [ "Searching for content using adhoc defined fields", "_z_r_f_quick_start.html#ZRFQSSearching", [
        [ "Selection on adhoc fields considered as Cstring", "_z_r_f_quick_start.html#ZRFQSSearchString", [
          [ "Establish the first collection of records ranks", "_z_r_f_quick_start.html#ZRFQSStr1", null ],
          [ "Refine the search", "_z_r_f_quick_start.html#ZRFQSStr2", null ]
        ] ],
        [ "Selection on adhoc binary fields", "_z_r_f_quick_start.html#ZRFQSSearchFloat", [
          [ "Establish the first collection of records ranks", "_z_r_f_quick_start.html#ZRFQSFloat1", null ],
          [ "Refine the search", "_z_r_f_quick_start.html#ZRFQSFloat2", null ]
        ] ],
        [ "Getting file's infra-data in XML format", "_z_r_f_quick_start.html#ZRFQSGetXML", null ]
      ] ],
      [ "Removing content from ZRandomFile", "_z_r_f_quick_start.html#ZRFQSRemoving", null ],
      [ "Getting statistical data", "_z_r_f_quick_start.html#ZRFQSGetStat", null ],
      [ "Converting an existing ZRandomFile to a ZMasterFile.", "_z_r_f_quick_start.html#ZRFQSConverting", null ],
      [ "Making some maintenance operations on file", "_z_r_f_quick_start.html#ZRFQSMaintenance", null ]
    ] ],
    [ "Bug List", "bug.html", null ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", "namespacemembers_eval" ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", "globals_eval" ],
        [ "Macros", "globals_defs.html", "globals_defs" ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_z_doc_phyaical_8cpp.html",
"build-zcontentlib-_desktop___qt__5__15__2___g_c_c__64bit-_debug_2moc__predefs_8h.html#a658d9ba84d429e748ce5f1905732c962",
"build-zcontentlib-_desktop___qt__5__15__2___g_c_c__64bit-_debug_2moc__predefs_8h.html#af8ad1ebe1976b0e31d68f9d223690126",
"build-zcontentlib-_desktop___qt__6__6__0___g_c_c__64bit-_debug_2moc__predefs_8h.html#a808f04c28bb0ef2d6b77dd66564ad351",
"build-zcontentlib-_desktop___qt__6__7__0___g_c_c__64bit-_debug_2moc__predefs_8h.html#a136189a915ba49e719dcffbeba8412fd",
"build-zcontentlib-_desktop___qt__6__7__0___g_c_c__64bit-_debug_2moc__predefs_8h.html#a9b10b4191fdb9929f3210b21744efc41",
"build-zcontentutils-_desktop___qt__6__7__0___g_c_c__64bit-_profile_2moc__predefs_8h.html#a190d0219caabccc0e05909f39bcb00d6",
"build-zcontentutils-_desktop___qt__6__7__0___g_c_c__64bit-_profile_2moc__predefs_8h.html#aa20ffd72afade34f989f1f18f1f66673",
"build-zcontentutils-_desktop___qt__6__7__0___g_c_c__64bit-_profile_2moc__ztypedlg_8cpp_source.html",
"class_file_generate_m_wn.html#aaad45221b1a8d0e15c37289dca437c7b",
"class_ui___raw_fields.html#a8567d3765eba55a12af59e37ad2a1305",
"class_ui___z_parsing_rules.html#a64e11bf9e5d80db245484d7814aeb8e7",
"class_z_block_descriptor.html#adace892de099983b9fd7dbd53020892f",
"class_z_content_visu_main.html#a568150283fd9c7a74e4759a89d33baad",
"class_z_doc_phy__2key.html#af38c7f77ba49f7ac1ea0765850d5c4e1",
"class_z_entry_point.html#aeaaf9ce58ce3a0af62553bf25172d416",
"class_z_identity.html",
"class_z_random_file_utils.html#a9c8958087b99038735da25c4f6cbc63b",
"class_z_raw_visu_main.html#abdc9be4f6d3641d429cd5e4dc30b5569",
"class_z_search_field.html#a49df1127b9660ddcccd97bf73a93f41d",
"class_z_type_list_button.html#aef5982fe1d96c51162ca48100e91d172",
"classzbs_1_1_field_desc___pack.html#afe16a3c803593a982e2809cf460c9dfb",
"classzbs_1_1_z_cpp_generate.html#a1992270d4497aa71026248bb8723dd20",
"classzbs_1_1_z_field_description.html#a3ae96f7b4265db8736fd112924c77e39",
"classzbs_1_1_z_general_parameters.html#ac3815bd307c14f2a6cbe4eda5716ae8c",
"classzbs_1_1_z_index_control_block.html#a9704f98b1b26e44f708df1a836968bde",
"classzbs_1_1_z_key_field_row.html#a113a40b0b7b21c482dcdc8e73ab6e16f",
"classzbs_1_1_z_master_control_block.html#a49ddc26eaa0f943bf214dfb84537f72a",
"classzbs_1_1_z_r_f_collection.html#ab541e39fd66109a80325ab64981575fc",
"classzbs_1_1_z_random_file.html#ad994829b0fd8fa0a9e309c8c458ee924",
"classzbs_1_1_z_s_s_l_console.html#a2f08c226b4b76df96c83a11cd3d2d816",
"classzbs_1_1_z_search_entity.html#a1fcf1afdcfddbe4fd72b074504d5db09",
"classzbs_1_1_z_search_entity_context.html#a159369e7ac06d0d6be46b7d87aa98f6f",
"classzbs_1_1_z_search_key_word.html#aefd467ba90dbf3c96f5224a0a7f4453d",
"classzbs_1_1_z_search_operand_base.html#ad3878ef167f33f606b9d9675fc02bb75",
"classzbs_1_1_z_search_parser.html#a852c36154b2a48468276256fe26c9431",
"classzbs_1_1_z_search_tokenizer.html#ad862d659010cf09eefd032a9e737ae15",
"classzdocphysical.html#ad6cfde7830a5a4b18861d968b826939d",
"globals_o.html",
"group___z_random_file_group.html#gae4da675406ddd238e8751d65b75bea3c",
"namespacezbs.html#a007a10b1cd838b6f8466d541dc053effa729912faf39dfd2671d7a6eca08c3cb2",
"namespacezbs.html#a7faeec794a404266d76b9cfe6932c7d5a3e615ce01ec31a64851ebd5b9cae160b",
"struct_z_lockid.html#a2f410bc68bc2fbdd8650a7179f142204",
"zcontent_2test__zrawfile_2main_8cpp.html#a20a9a3bfa55d9dcee5212cac97ad3f0cab7022fdb96fffd404d9a2e54cfaab264",
"zcontentutils_2moc__predefs_8h.html#a20fcee7a683d69340d8c3d126e5a7f12",
"zcontentutils_2moc__predefs_8h.html#aaab7817ee2e4bb88b5178e101e7ab2a6",
"zcontentutils_2ui__texteditmwn_8h_source.html",
"zdatatype_8h.html#ab22bc09ade4982e9354dc642b138a06d",
"zifgeneric_8h.html#a27466c591b8988513791c706a00b652d",
"zmailabstract_8h.html#a113c69042ba052bd95022efa082f1fdeaffc01ff121100a4ccd99d34e77461f9a",
"znetcommon_8h.html",
"zrfutilities_8h.html",
"zsearchparser_8h.html#ab0a00ebe6b609b46c9bfcc2112d41a9ca9ddc45b12b5057e4f80dc7c17b6d8979",
"zsearchparsertype_8h.html#a007a10b1cd838b6f8466d541dc053effafe6ac71b438c3cb9e0b30ae9cd07642a",
"zsslfunctions_8h.html#aaf67f28995790d8c6b48af923c056ea2",
"ztest__zrandomfile_2main_z_r_f_collection_8cpp.html#a9f6fa8900c00756f7875c94b6565e096a51c9b5166139024b94384c1278709449"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';