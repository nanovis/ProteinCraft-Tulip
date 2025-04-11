INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipGUIInclude})

ADD_LIBRARY(ProteinCraftLayout-${Tulip_VERSION} 
            SHARED 
            BipartiteSubgraph.cpp
            BinderTargetSubgraph.cpp

            BipartiteLayout.cpp
            ConnectedLayout.cpp
            InteractionFilter.cpp)

TARGET_LINK_LIBRARIES(ProteinCraftLayout-${Tulip_VERSION} ${LibTulipCoreName} ${LibTulipGUIName})

TARGET_LINK_LIBRARIES(ProteinCraftLayout-${Tulip_VERSION} GraphPerspective-${Tulip_VERSION})

SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET_TARGET_PROPERTIES(ProteinCraftLayout-${Tulip_VERSION} PROPERTIES INSTALL_RPATH "$ORIGIN/perspective")

TULIP_INSTALL_PLUGIN(ProteinCraftLayout-${Tulip_VERSION} ${TulipPluginsInstallDir})
