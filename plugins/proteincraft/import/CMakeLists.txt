INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipGUIInclude})
ADD_LIBRARY(AF2igImport-${Tulip_VERSION} SHARED AF2igImport.cpp)
TARGET_LINK_LIBRARIES(AF2igImport-${Tulip_VERSION} ${LibTulipCoreName} ${LibTulipGUIName})
TULIP_INSTALL_PLUGIN(AF2igImport-${Tulip_VERSION} ${TulipPluginsInstallDir})
