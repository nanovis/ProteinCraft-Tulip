IF(WIN32)  
  add_compile_definitions("DLL_TULIP")
ENDIF(WIN32)
add_compile_definitions("TULIP_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}")

ADD_CORE_FILES(.)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)

