IF(WIN32)
   add_compile_definitions("DLL_TULIP_GL")
ENDIF(WIN32)

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(bitmaps)
ADD_SUBDIRECTORY(src)
