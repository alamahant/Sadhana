#include "constants.h"

namespace Constants {


#ifndef FLATPAK_BUILD
const QString appDirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Sadhana";
#else
const QString appDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Sadhana";
#endif


const QString imagesPath = appDirPath + "/images";
const QString audioPath = appDirPath + "/audio";
const QString customModulesPath = appDirPath + "/CustomModules";
const QString filesPath = appDirPath + "/files";
const QString journalPath = appDirPath + "/journal";
const QString defaultModuleDataPath = appDirPath + "/DefaultModules";

}

