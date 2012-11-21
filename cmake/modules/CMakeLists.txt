# install the cmake files

set(cmakeFiles FindChoqok.cmake FindQJson.cmake FindQtOAuth.cmake FindKFacebook.cmake)
install( FILES ${cmakeFiles} DESTINATION ${DATA_INSTALL_DIR}/cmake/modules )