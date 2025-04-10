INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipGUIInclude})
ADD_LIBRARY(SyncMol-${Tulip_VERSION} SHARED SyncMol.cpp)
TARGET_LINK_LIBRARIES(SyncMol-${Tulip_VERSION} ${LibTulipCoreName} ${LibTulipGUIName})
TULIP_INSTALL_PLUGIN(SyncMol-${Tulip_VERSION} ${TulipPluginsInstallDir})
