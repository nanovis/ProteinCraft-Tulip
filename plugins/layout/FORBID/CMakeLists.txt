INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude})
FILE(GLOB LIB_SRCS "*.c*")

ADD_LIBRARY(FORBID-${Tulip_VERSION} SHARED ${LIB_SRCS})
TARGET_LINK_LIBRARIES(FORBID-${Tulip_VERSION}  ${LibTulipCoreName})

TULIP_INSTALL_PLUGIN(FORBID-${Tulip_VERSION} ${TulipPluginsInstallDir})
