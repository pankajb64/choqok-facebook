# install the cmake files

set(cmakeFiles FindChoqok.cmake FindQJson.cmake FindQtOAuth.cmake FindKFBAPI.cmake)
install( FILES ${cmakeFiles} DESTINATION ${DATA_INSTALL_DIR}/cmake/modules )
