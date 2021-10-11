#ifndef ZJOURNAL_DOC_H
#define ZJOURNAL_DOC_H

namespace zbs {
/** @defgoup JournalingGroup Journaling for master files
 * @ingroup ZMasterFileGroup
 *
 * Journaling for ZMasterFile allows to :
 * - store each data modification in a journal file
 * - rollback modification on demand
 * - maintain an updated remote copy of ZMasterFile and its indexes
 *
 *
 *
 * @section JNLSettingOn Setting journaling on for a file : ZMasterFile::setJournalingOn()
 *
 * setting journaling on for a ZMasterFile is done using ZMasterFile::setJournalingOn() method.
 *
 * Jounaling parameters are stored within ZMasterFile MCB header.
 * The following parameters must be set :
 * - Journal file directory path :
 * Directory path must point to a local directory that will contain journal file.
 * If this parameter is omitted, journal file will be stored in the same directory as ZMasterFile's current directory.
 *
 * Once having being set on for the file, Journaling is automatically started when file is opened for the next first time,
 * and will remain active even if file is closed (to avoid possible multiple thread starts) up until ZMasterFile object is deleted.
 * So that, there is no need to use setJournalingOn() routine again for further use.
 *
 * @subsection JNLMechanism Journaling mechanism
 *
 *
 *
 * @section JNLSettingOff Setting journaling off : ZMasterFile::setJournalOff()
 *
 * Setting journaling off using ZMasterFile::setJournalOff() disable journaling for this session
 * and for further sessions for this ZMasterFile until journaling is enabled again
 *
 *
 * @section JNLResetting Resetting journaling : ZMasterFile::reset()
 *
 *
 *
 */



}// zbs


#endif // ZJOURNAL_DOC_H

