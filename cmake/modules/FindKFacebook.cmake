# - Try to find the KFacebook library
# Once done this will define
#
#  KFacebook_FOUND - system has kfacebook
#  KFACEBOOK_INCLUDE_DIR - the kfacebook include directory
#  KFACEBOOK_LIBRARIES - Link these to use kfacebook



if(KFACEBOOK_INCLUDE_DIR AND KFACEBOOK_LIBRARIES)

  # read from cache
  set(KFacebook_FOUND TRUE)

else(KFACEBOOK_INCLUDE_DIR AND KFACEBOOK_LIBRARIES)

  FIND_PATH(KFACEBOOK_INCLUDE_DIR
    NAMES
    kfacebook/libkfacebook_export.h
    PATHS 
    ${KDE4_INCLUDE_DIR}
    ${INCLUDE_INSTALL_DIR}
    )
  
  FIND_LIBRARY(KFACEBOOK_LIBRARIES
    NAMES
    kfacebook
    PATHS
    ${KDE4_LIB_DIR}
    ${LIB_INSTALL_DIR}
    )
  if(KFACEBOOK_INCLUDE_DIR AND KFACEBOOK_LIBRARIES)
    set(KFacebook_FOUND TRUE)
  endif(KFACEBOOK_INCLUDE_DIR AND KFACEBOOK_LIBRARIES)

  if(KFacebook_FOUND)
    if(NOT KFacebook_FIND_QUIETLY)
      message(STATUS "Found KFacebook: ${KFACEBOOK_LIBRARIES}")
    endif(NOT KFacebook_FIND_QUIETLY)
  else(KFacebook_FOUND)
    if(KFacebook_FIND_REQUIRED)
      if(NOT KFACEBOOK_INCLUDE_DIR)
	message(FATAL_ERROR "Could not find KFacebook includes.")
      endif(NOT KFACEBOOK_INCLUDE_DIR)
      if(NOT KFACEBOOK_LIBRARIES)
	message(FATAL_ERROR "Could not find KFacebook library.")
      endif(NOT KFACEBOOK_LIBRARIES)
    else(KFacebook_FIND_REQUIRED)
      if(NOT KFACEBOOK_INCLUDE_DIR)
        message(STATUS "Could not find KFacebook includes.")
      endif(NOT KFACEBOOK_INCLUDE_DIR)
      if(NOT KFACEBOOK_LIBRARIES)
        message(STATUS "Could not find KFacebook library.")
      endif(NOT KFACEBOOK_LIBRARIES)
    endif(KFacebook_FIND_REQUIRED)
  endif(KFacebook_FOUND)

endif(KFACEBOOK_INCLUDE_DIR AND KFACEBOOK_LIBRARIES)
