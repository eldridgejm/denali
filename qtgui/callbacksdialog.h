#ifndef DENALI_QTGUI_CALLBACKSDIALOG_H
#define DENALI_QTGUI_CALLBACKSDIALOG_H

#include <QDialog>
#include "ui_CallbacksDialog.h"

#include <boost/shared_ptr.hpp>
#include <string>

class CallbacksDialog : public QDialog
{
    Q_OBJECT

public:

    CallbacksDialog(QWidget* parent);

    std::string getInfoCallback();
    std::string getTreeCallback();
    std::string getAsyncCallback();

    bool runInfoOnSelection();
    bool runTreeOnSelection();
    bool runAsyncOnSelection();

    bool provideInfoSubtree();
    bool provideTreeSubtree();
    bool provideAsyncSubtree();

public slots:
    void setInfoCallback();
    void setTreeCallback();
    void setAsyncCallback();

    void clearInfoCallback();
    void clearTreeCallback();
    void clearAsyncCallback();

private:

    Ui::CallbacksDialog _dialog;    
    
};

#endif
