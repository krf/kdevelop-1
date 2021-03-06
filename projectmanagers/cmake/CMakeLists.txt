project(cmakemanager)
add_definitions(-DTRANSLATION_DOMAIN=\"kdevcmake\")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parser)

add_subdirectory(tests)
add_subdirectory(icons)

# enable this if you want to have the cmake debug visitor run on each CMakeLists.txt
# the debug visitor prints out the Ast for the CMakeLists.txt file.
#add_definitions( -DCMAKEDEBUGVISITOR )

add_definitions( -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set( cmakecommon_SRCS
  parser/cmListFileLexer.c
#   parser/astfactory.cpp
#   parser/cmakelistsparser.cpp
#   parser/cmakeast.cpp
#   parser/cmakecondition.cpp
#   parser/cmakeprojectvisitor.cpp
#   parser/variablemap.cpp
#   parser/cmakedebugvisitor.cpp
  parser/cmakecachereader.cpp
  parser/cmakelistsparser.cpp
  parser/cmakeduchaintypes.cpp
#   parser/cmakeparserutils.cpp
#   parser/cmakeduchaintypes.cpp
#   parser/generationexpressionsolver.cpp
  cmakeutils.cpp
  cmakeextraargumentshistory.cpp
  cmakebuilddirchooser.cpp
  debug.cpp

 )

set( cmakecommon_UI
  cmakebuilddirchooser.ui
)

set( cmakemanager_SRCS
  testing/ctestutils.cpp
  testing/ctestfindjob.cpp
  testing/ctestrunjob.cpp
  testing/ctestsuite.cpp
  testing/qttestdelegate.cpp
  cmakeimportjsonjob.cpp
  cmakenavigationwidget.cpp
  cmakemanager.cpp
  cmakemodelitems.cpp
  duchain/cmakeparsejob.cpp
  duchain/usebuilder.cpp
  duchain/declarationbuilder.cpp
  duchain/contextbuilder.cpp
  cmakecodecompletionmodel.cpp
#   cmakecommitchangesjob.cpp
#   cmakeimportjob.cpp
#   cmakeedit.cpp
  debug.cpp
)

set( cmakemanager_UI
  cmakepossibleroots.ui
)

set( cmakesettings_SRCS
  settings/cmakepreferences.cpp
  settings/cmakecachemodel.cpp
  settings/cmakecachedelegate.cpp
  settings/cmakecachemodel.cpp
  debug.cpp
)
ki18n_wrap_ui(cmakesettings_SRCS settings/cmakebuildsettings.ui)

set( cmakedoc_SRCS
  cmakedocumentation.cpp
  cmakehelpdocumentation.cpp
)

remove_definitions( -DQT_NO_STL )

if(WIN32)
    add_definitions(-DYY_NO_UNISTD_H)
endif()

# Note: This library doesn't follow API/ABI/BC rules and shouldn't have a SOVERSION
#       Its only purpose is to support the plugin without needing to add all source files
#       to the plugin target

ki18n_wrap_ui( cmakecommon_SRCS ${cmakecommon_UI} )
add_library( kdevcmakecommon SHARED ${cmakecommon_SRCS} )
target_link_libraries( kdevcmakecommon
                        KF5::TextEditor KDev::Interfaces KDev::Project KDev::Util
                        KDev::Language
                        )
generate_export_header(kdevcmakecommon EXPORT_FILE_NAME cmakecommonexport.h)

ki18n_wrap_ui( cmakemanager_SRCS ${cmakemanager_UI} )
add_library( kdevcmakemanagernosettings STATIC ${cmakemanager_SRCS})
target_link_libraries( kdevcmakemanagernosettings KF5::KIOWidgets KDev::Util
                        KDev::Interfaces kdevcmakecommon kdevmakefileresolver
                        KDev::Project KDev::Language
                        KDev::OutputView KF5::TextEditor Qt5::Concurrent)

kdevplatform_add_plugin(kdevcmakemanager JSON kdevcmakemanager.json SOURCES ${cmakemanager_SRCS} ${cmakesettings_SRCS})
target_link_libraries( kdevcmakemanager  KF5::KIOWidgets KDev::Util
                        KDev::Interfaces kdevcmakecommon kdevmakefileresolver
                        KDev::Project KDev::Language
                        KDev::OutputView KF5::TextEditor Qt5::Concurrent)

kdevplatform_add_plugin(kdevcmakedocumentation JSON kdevcmakedocumentation.json SOURCES ${cmakedoc_SRCS})
target_link_libraries( kdevcmakedocumentation
                        KDev::Interfaces kdevcmakecommon
                        KDev::Project KDev::Language
                        KDev::Documentation
                        KF5::TextEditor)

install(TARGETS kdevcmakecommon ${INSTALL_TARGETS_DEFAULT_ARGS} )
