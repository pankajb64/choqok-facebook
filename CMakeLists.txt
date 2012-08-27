project(choqok-facebook)
cmake_minimum_required(VERSION 2.6)
find_package(KDE4 REQUIRED 4.4)
find_package(Choqok REQUIRED)
find_package(QJson REQUIRED)
find_package(QCA2 REQUIRED)
find_package(QtOAuth REQUIRED)

set(CHOQOK_FACEBOOK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CHOQOK_FACEBOOK_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CHOQOK_FACEBOOK_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(KdepimLibs REQUIRED)
find_package(KFacebook REQUIRED)

include (KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${KDE4_INCLUDES})
include_directories(${CHOQOK_INCLUDES})
include_directories(${CHOQOK_INCLUDE_DIR} ${CHOQOK_INCLUDE_DIR}/choqok ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QCA2_INCLUDE_DIR} ${QTOAUTH_INCLUDE_DIR} ${QJSON_INCLUDE_DIR})


option(KDEPIM_44_COMPAT "Don't crash when using KDEPIM runtime 4.4.x, at the cost of using deprecated KCal API" TRUE)
if(KDEPIM_44_COMPAT)
  add_definitions(-DWANT_DEPRECATED_KCAL_API)
endif(KDEPIM_44_COMPAT)

set(CHOQOK_FACEBOOK_INCLUDES
${CHOQOK_INCLUDES}
${CHOQOK_FACEBOOK_SOURCE_DIR}/facebook
${CHOQOK_FACEBOOK_BINARY_DIR}/facebook
)

add_subdirectory(facebook)
add_subdirectory(cmake)
