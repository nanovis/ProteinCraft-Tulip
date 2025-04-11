INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipGUIInclude})

ADD_LIBRARY(ProteinCraftGeneral-${Tulip_VERSION} 
            SHARED 
            OpenRINGForSelectedNode.cpp)

TARGET_LINK_LIBRARIES(ProteinCraftGeneral-${Tulip_VERSION} ${LibTulipCoreName} ${LibTulipGUIName})

TARGET_LINK_LIBRARIES(ProteinCraftGeneral-${Tulip_VERSION} GraphPerspective-${Tulip_VERSION})

SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET_TARGET_PROPERTIES(ProteinCraftGeneral-${Tulip_VERSION} PROPERTIES INSTALL_RPATH "$ORIGIN/perspective")

TULIP_INSTALL_PLUGIN(ProteinCraftGeneral-${Tulip_VERSION} ${TulipPluginsInstallDir})
