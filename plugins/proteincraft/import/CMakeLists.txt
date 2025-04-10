INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipGUIInclude})

ADD_LIBRARY(AF2igUmapImport-${Tulip_VERSION} SHARED AF2igUmapImport.cpp)

TARGET_LINK_LIBRARIES(AF2igUmapImport-${Tulip_VERSION} ${LibTulipCoreName} ${LibTulipGUIName})

TARGET_LINK_LIBRARIES(AF2igUmapImport-${Tulip_VERSION} GraphPerspective-${Tulip_VERSION})

SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET_TARGET_PROPERTIES(AF2igUmapImport-${Tulip_VERSION} PROPERTIES INSTALL_RPATH "$ORIGIN/perspective")

TULIP_INSTALL_PLUGIN(AF2igUmapImport-${Tulip_VERSION} ${TulipPluginsInstallDir})
