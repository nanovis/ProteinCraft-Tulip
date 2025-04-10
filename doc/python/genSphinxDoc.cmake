# Turn back the CMAKE_LIBRARY_PATH string into a list
STRING(REPLACE "," ";" CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH}")

# On windows, we need to add the paths to all the dll dependencies of the tulip Python modules
# in the PATH environment variable, otherwise the import of the tulip modules will fail
IF(WIN32)
  SET(DEPENDENCIES_PATHS "${BIN_DIR}/../../library/tulip-core/src"
                         "${BIN_DIR}/../../library/tulip-gui/src"
                         "${BIN_DIR}/../../library/tulip-ogl/src"
                         "${BIN_DIR}/../../library/tulip-ogdf/src/"
                         "${BIN_DIR}/../../library/tulip-python/src/"
                         "${BIN_DIR}/../../thirdparty/gzstream"
                         "${BIN_DIR}/../../thirdparty/ftgl"
                         "${BIN_DIR}/../../thirdparty/OGDF"
                         "${BIN_DIR}/../../thirdparty/libtess2"
                         "${CMAKE_LIBRARY_PATH}"
                         "${QT_BIN_DIR}")

  SET(ENV{PATH} "${DEPENDENCIES_PATHS};$ENV{PATH}")
  SET(ENV{QT_QPA_PLATFORM_PLUGIN_PATH} "${QT_BIN_DIR}/../plugins/platforms")

ENDIF(WIN32)

# Add the paths of the tulip Python modules in the PYTHONPATH environment variable
# in order for the interpreter to import them
SET(PYTHON_PATHS "${BIN_DIR}/../../library/tulip-python/bindings/tulip-core/tulip_module"
                 "${BIN_DIR}/../../library/tulip-python/bindings/tulip-gui/tulipgui_module"
                 "${BIN_DIR}/../../library/tulip-python/modules"
                 "${SRC_DIR}")

# Add the path of the sip Python module as the tulip modules depend on it
SET(PYTHON_PATHS "${TULIP_PYTHON_NATIVE_FOLDER}"
                   "${PYTHON_PATHS}")

# On non windows systems, the character separating the paths in an environment variable
# is a ':' not a ';' (generated by CMake when turning a list into string)
CMAKE_PATH(CONVERT "${PYTHON_PATHS}" TO_NATIVE_PATH_LIST PYTHON_PATHS NORMALIZE)

SET(ENV{PYTHONPATH} "${PYTHON_PATHS}")

SET(ENV{TULIP_BUILD_DIR} "${TULIP_BUILD_DIR}")
SET(ENV{TULIP_SOURCE_DIR} "${TULIP_SOURCE_DIR}")
SET(ENV{TULIP_PYTHON_PLUGINS_DIR} "${BIN_DIR}/../../library/tulip-python/bindings/tulip-core/tulip_module/tulip/plugins")
SET(ENV{TULIPGUI_PYTHON_PLUGINS_DIR} "${BIN_DIR}/../../library/tulip-python/bindings/tulip-gui/tulipgui_module/tulipgui/plugins")

# Fix an encoding issue on Mac OS
IF(APPLE)
  SET(ENV{LC_ALL} "en_EN.UTF-8")
ENDIF(APPLE)

# Generate Tulip plugins documentation file from embedded plugins metadata
EXECUTE_PROCESS(COMMAND ${Python_EXECUTABLE} ${BIN_DIR}/gen_plugins_doc.py WORKING_DIRECTORY ${BIN_DIR} COMMAND_ERROR_IS_FATAL ANY)

# Everything is now set up, we can generate the documentation
EXECUTE_PROCESS(COMMAND ${SPHINX_EXECUTABLE} -c ${BIN_DIR} -b html -E -d ${BIN_DIR}/doctrees ${SRC_DIR} ${BIN_DIR}/html)
