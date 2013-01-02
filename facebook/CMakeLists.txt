
include_directories(
${CHOQOK_INCLUDES}
${KFBAPI_INCLUDE_DIR}
${CMAKE_CURRENT_BINARY_DIR}
)

set( choqok_facebook_SRCS
facebookaccount.cpp
facebookmicroblog.cpp
facebookeditaccountwidget.cpp
facebookpostwidget.cpp
facebookutil.cpp
facebookviewdialog.cpp
facebookwhoiswidget.cpp
facebookcomposerwidget.cpp
facebookinputdialog.cpp
)

kde4_add_ui_files(choqok_facebook_SRCS
facebookeditaccount_base.ui
)

kde4_add_plugin(choqok_facebook ${choqok_facebook_SRCS})

target_link_libraries(choqok_facebook ${KDE4_KDECORE_LIBS}   ${KDE4_KDEUI_LIBS}   ${QT_QTGUI_LIBRARY}   ${QT_QTWEBKIT_LIBRARY}   ${KDE4_KDEWEBKIT_LIBS}   ${KDE4_KIO_LIBS}   ${QJSON_LIBRARIES}   ${QJSON_LIBRARY}   ${KDEPIMLIBS_KABC_LIBS}   ${KDEPIMLIBS_KPIMUTILS_LIBS}   ${KDEPIMLIBS_KMIME_LIBS} ${KFBAPI_LIBRARIES} ${CHOQOK_LIBRARIES})
#target_link_libraries(choqok_facebook ${KDE4_KDEUI_LIBS} ${LIBKFBAPI_LIBRARIES} choqok)

# Install


install(TARGETS choqok_facebook DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES choqok_facebook.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(icons)
