#include "colormapdialog.h"

#include <QtGui>

ColorMapDialog::ColorMapDialog(QWidget* parent) :
        QDialog(parent) 
{
    _dialog.setupUi(this);

    connect(_dialog.pushButtonBrowse, SIGNAL(clicked()),
            this, SLOT(openColorMapFile()));
}


void ColorMapDialog::openColorMapFile()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Open Color Map File"), "", tr("Files(*.colors)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    _dialog.lineEditColorMapPath->setText(qfilename);
}


std::string ColorMapDialog::getColorMapPath() const {
    return _dialog.lineEditColorMapPath->text().toUtf8().constData();
}


int ColorMapDialog::getReductionIndex() const {
    return _dialog.comboBoxReduction->currentIndex();
}


bool ColorMapDialog::isParentContributorChecked() const {
    return _dialog.radioButtonParentContributor->isChecked();
}


bool ColorMapDialog::isChildContributorChecked() const {
    return _dialog.radioButtonChildContributor->isChecked();
}


bool ColorMapDialog::isBothContributorChecked() const {
    return _dialog.radioButtonBothContributor->isChecked();
}


bool ColorMapDialog::includeMembers() const {
    return _dialog.checkBoxIncludeMembers->isChecked();
}
