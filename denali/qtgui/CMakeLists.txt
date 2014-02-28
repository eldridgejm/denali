
qt4_wrap_ui(UI_SRCS MainWindow.ui)
qt4_wrap_cpp(MOC_SRCS mainwindow.h)

add_executable(denali main.cpp ${MOC_SRCS} ${UI_SRCS})
target_link_libraries(denali ${QT_LIBRARIES} ${VTK_LIBRARIES} QVTK)
