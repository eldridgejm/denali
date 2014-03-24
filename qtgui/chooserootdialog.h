#ifndef DENALI_QTGUI_CHOOSEROOTDIALOG_H
#define DENALI_QTGUI_CHOOSEROOTDIALOG_H

#include <QDialog>
#include "ui_ChooseRootDialog.h"

#include <boost/shared_ptr.hpp>
#include <string>

class ChooseRootDialog : public QDialog
{
    Q_OBJECT

public:

    ChooseRootDialog(QWidget* parent);

public slots:

    bool isMinimumNodeChecked() const;
    bool isMaximumNodeChecked() const;
    bool isOtherNodeChecked() const;
    unsigned int getOtherNode() const;

private:

    Ui::ChooseRootDialog _dialog;    
    

};

#endif
