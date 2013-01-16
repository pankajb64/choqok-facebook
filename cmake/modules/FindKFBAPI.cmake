# - Try to find the KFacebook library
# Once done this will define
#
#  KFacebook_FOUND - system has kfbapi
#  KFBAPI_INCLUDE_DIR - the kfbapi include directory
#  KFBAPI_LIBRARIES - Link these to use kfbapi



if(KFBAPI_INCLUDE_DIR AND KFBAPI_LIBRARIES)

  # read from cache
  set(KFacebook_FOUND TRUE)

else(KFBAPI_INCLUDE_DIR AND KFBAPI_LIBRARIES)

  FIND_PATH(KFBAPI_INCLUDE_DIR
    NAMES
    libkfbapi/libkfbapi_export.h
    PATHS 
    ${KDE4_INCLUDE_DIR}
    ${INCLUDE_INSTALL_DIR}
    )
  
  FIND_LIBRARY(KFBAPI_LIBRARIES
    NAMES
    kfbapi
    PATHS
    ${KDE4_LIB_DIR}
    ${LIB_INSTALL_DIR}
    )
  if(KFBAPI_INCLUDE_DIR AND KFBAPI_LIBRARIES)
    set(KFacebook_FOUND TRUE)
  endif(KFBAPI_INCLUDE_DIR AND KFBAPI_LIBRARIES)

  if(KFacebook_FOUND)
    if(NOT KFacebook_FIND_QUIETLY)
      message(STATUS "Found KFacebook: ${KFBAPI_LIBRARIES}")
    endif(NOT KFacebook_FIND_QUIETLY)
  else(KFacebook_FOUND)
    if(KFacebook_FIND_REQUIRED)
      if(NOT KFBAPI_INCLUDE_DIR)
	message(FATAL_ERROR "Could not find KFacebook includes.")
      endif(NOT KFBAPI_INCLUDE_DIR)
      if(NOT KFBAPI_LIBRARIES)
	message(FATAL_ERROR "Could not find KFacebook library.")
      endif(NOT KFBAPI_LIBRARIES)
    else(KFacebook_FIND_REQUIRED)
      if(NOT KFBAPI_INCLUDE_DIR)
        message(STATUS "Could not find KFacebook includes.")
      endif(NOT KFBAPI_INCLUDE_DIR)
      if(NOT KFBAPI_LIBRARIES)
        message(STATUS "Could not find KFacebook library.")
      endif(NOT KFBAPI_LIBRARIES)
    endif(KFacebook_FIND_REQUIRED)
  endif(KFacebook_FOUND)

endif(KFBAPI_INCLUDE_DIR AND KFBAPI_LIBRARIES)
