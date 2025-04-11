INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipGUIInclude})

ADD_LIBRARY(ProteinCraftImport-${Tulip_VERSION} 
            SHARED 
            AF2igUmapImport.cpp
            RingImport.cpp)

TARGET_LINK_LIBRARIES(ProteinCraftImport-${Tulip_VERSION} ${LibTulipCoreName} ${LibTulipGUIName})

TARGET_LINK_LIBRARIES(ProteinCraftImport-${Tulip_VERSION} GraphPerspective-${Tulip_VERSION})

SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET_TARGET_PROPERTIES(ProteinCraftImport-${Tulip_VERSION} PROPERTIES INSTALL_RPATH "$ORIGIN/perspective")

TULIP_INSTALL_PLUGIN(ProteinCraftImport-${Tulip_VERSION} ${TulipPluginsInstallDir})
