FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

set(SOURCES main.cpp 
            mainwindow.cpp 
            landscape_interactor.cpp 
            colormapdialog.cpp 
            callbacksdialog.cpp)

set(HEADERS mainwindow.h 
            colormapdialog.h 
            callbacksdialog.h)

qt4_wrap_ui(UI_SRCS MainWindow.ui 
                    ColorMapDialog.ui
                    CallbacksDialog.ui)

qt4_wrap_cpp(HEADERS_MOC ${HEADERS})

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
add_executable(denali ${SOURCES} ${HEADERS_MOC} ${UI_SRCS})
target_link_libraries(denali ${QT_LIBRARIES} ${VTK_LIBRARIES} QVTK)
