// Copyright (c) 2014, Justin Eldridge, The Ohio State University
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
