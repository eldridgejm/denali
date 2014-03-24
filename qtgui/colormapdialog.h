#ifndef DENALI_QTGUI_COLORMAPDIALOG_H
#define DENALI_QTGUI_COLORMAPDIALOG_H

#include <QDialog>
#include "ui_ColorMapDialog.h"

#include <boost/shared_ptr.hpp>
#include <string>

class ColorMapDialog : public QDialog
{
    Q_OBJECT

public:

    ColorMapDialog(QWidget* parent);

    std::string getColorMapPath() const;
    int getReductionIndex() const;

public slots:
    void openColorMapFile();

private:

    Ui::ColorMapDialog _dialog;    
    

};

#endif
