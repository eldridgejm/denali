#ifndef DENALI_QTGUI_MAINWINDOW_H
#define DENALI_QTGUI_MAINWINDOW_H

#include <boost/shared_ptr.hpp>

#include <QMainWindow>
#include <QtGui>

#include "ui_MainWindow.h"
#include "landscape_context.h"
#include "landscape_interface.h"

class MainWindow : public QMainWindow, public LandscapeEventObserver
{
    Q_OBJECT

public:
    
    MainWindow();

    void setContext(LandscapeContext*);
    void receiveCellSelection(unsigned int);

public slots:
    void setStatus(const std::string&);
    void appendStatus(const std::string&);

    void openContourTreeFile();
    void renderLandscape();
    void changeLandscapeRoot();
    void updateCellSelection(unsigned int);
    void enablePersistenceSlider();
    void updatePersistence(int);
    void enableRefineSubtree();
    void refineSubtree();
    void cellSelectionCallback(unsigned int);
    void treeBuilderCallback();
    void enableLoadWeightMap();
    void loadWeightMapFile();

signals:
    void landscapeChanged();
    void cellSelected(unsigned int);

private:

    Ui::MainWindow _mainwindow;
    boost::shared_ptr<LandscapeContext> _landscape_context;
    boost::shared_ptr<LandscapeInterface> _landscape_interface;

    int _max_persistence_slider_value;
    unsigned int _cell_selection;

};


#endif
