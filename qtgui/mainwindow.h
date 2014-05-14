#ifndef DENALI_QTGUI_MAINWINDOW_H
#define DENALI_QTGUI_MAINWINDOW_H

#include <boost/shared_ptr.hpp>

#include <QMainWindow>
#include <QtGui>

#include "colormapdialog.h"
#include "callbacksdialog.h"
#include "chooserootdialog.h"
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

    std::string prepareCallback(std::string callback_path,
                                unsigned int cell,
                                QTemporaryFile& tempfile,
                                bool provide_subtree = false);

    std::string runSynchronousCallback(std::string callback_path, 
                                       unsigned int cell, 
                                       bool provide_subtree = false);

    void runAsynchronousCallback(std::string callback_path, 
                                       unsigned int cell, 
                                       bool provide_subtree = false);

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
    void disableRefineSubtree();

    void refineSubtree();

    void enableLoadWeightMap();
    void enableClearWeightMap();
    void disableClearWeightMap();
    void loadWeightMapFile();
    void clearWeightMap();

    void enableConfigureColorMap();
    void enableClearColorMap();
    void disableClearColorMap();
    void configureColorMap();
    void clearColorMap();

    void configureCallbacks();
    void runInfoCallback();
    void runTreeCallback();
    void runAsyncCallback();

    void updateCallbackAvailability();
    void runCallbacksOnSelection();
    void enableInfoCallback();
    void enableTreeCallback();
    void enableAsyncCallback();
    void disableInfoCallback();
    void disableTreeCallback();
    void disableAsyncCallback();

    void enableRebaseLandscape();
    void disableRebaseLandscape();
    void rebaseLandscape();

    void enableExpandLandscape();
    void disableExpandLandscape();
    void expandLandscape();

    void enableChooseRoot();
    void chooseRoot();

signals:
    void landscapeChanged();
    void cellSelected(unsigned int);

private:

    Ui::MainWindow _mainwindow;
    boost::shared_ptr<LandscapeContext> _landscape_context;
    boost::shared_ptr<LandscapeInterface> _landscape_interface;

    int _max_persistence_slider_value;
    unsigned int _cell_selection;

    ColorMapDialog* _color_map_dialog;
    CallbacksDialog* _callbacks_dialog;
    ChooseRootDialog* _choose_root_dialog;

    bool _use_color_map;

    std::string _filename;

    enum SelectedReduction 
    {
        MAXIMUM,
        MINIMUM,
        MEAN,
        COUNT,
        VARIANCE,
        COVARIANCE,
        CORRELATION
    };

    int _progress_wait_time;

};


#endif
