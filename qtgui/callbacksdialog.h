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
    std::string getVoidCallback();

    bool runInfoOnSelection();
    bool runTreeOnSelection();
    bool runVoidOnSelection();

    bool provideInfoSubtree();
    bool provideTreeSubtree();
    bool provideVoidSubtree();

public slots:
    void setInfoCallback();
    void setTreeCallback();
    void setVoidCallback();

    void clearInfoCallback();
    void clearTreeCallback();
    void clearVoidCallback();

private:

    Ui::CallbacksDialog _dialog;    
    
};

#endif
