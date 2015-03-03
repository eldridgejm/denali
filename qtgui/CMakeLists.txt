FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

set(SOURCES main.cpp 
            mainwindow.cpp 
            landscape_interactor.cpp 
            colormapdialog.cpp 
            callbacksdialog.cpp
            chooserootdialog.cpp)

set(HEADERS mainwindow.h 
            colormapdialog.h 
            callbacksdialog.h
            chooserootdialog.h)

qt4_wrap_ui(UI_SRCS MainWindow.ui 
                    ColorMapDialog.ui
                    CallbacksDialog.ui
                    ChooseRootDialog.ui)

qt4_wrap_cpp(HEADERS_MOC ${HEADERS})

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

IF(WIN32)
    add_executable(denali WIN32 ${SOURCES} ${HEADERS_MOC} ${UI_SRCS})
ELSE()
    add_executable(denali ${SOURCES} ${HEADERS_MOC} ${UI_SRCS})
ENDIF()


IF(${VTK_VERSION_MAJOR} VERSION_GREATER 5)
    target_link_libraries(denali ${QT_LIBRARIES} ${VTK_LIBRARIES})
ELSE()
    message("Linking qvtk")
    target_link_libraries(denali ${QT_LIBRARIES} ${VTK_LIBRARIES} QVTK)
ENDIF()

install(TARGETS denali DESTINATION bin)
install(FILES startlogo.png DESTINATION share/denali)
