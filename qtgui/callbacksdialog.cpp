#include "callbacksdialog.h"

#include <QtGui>

CallbacksDialog::CallbacksDialog(QWidget* parent) :
        QDialog(parent) 
{
    _dialog.setupUi(this);

    connect(_dialog.pushButtonBrowseInfo, SIGNAL(clicked()),
            this, SLOT(setInfoCallback()));

    connect(_dialog.pushButtonBrowseTree, SIGNAL(clicked()),
            this, SLOT(setTreeCallback()));

    connect(_dialog.pushButtonBrowseVoid, SIGNAL(clicked()),
            this, SLOT(setVoidCallback()));

}


void CallbacksDialog::setInfoCallback()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Select Info Callback"), "", tr("Files(*)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    _dialog.lineEditInfoCallback->setText(qfilename);
}


void CallbacksDialog::setTreeCallback()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Select Tree Callback"), "", tr("Files(*)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    _dialog.lineEditTreeCallback->setText(qfilename);
}


void CallbacksDialog::setVoidCallback()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Select Void Callback"), "", tr("Files(*)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    _dialog.lineEditVoidCallback->setText(qfilename);
}


std::string CallbacksDialog::getInfoCallback() {
    return _dialog.lineEditInfoCallback->text().toUtf8().constData();
}


std::string CallbacksDialog::getTreeCallback() {
    return _dialog.lineEditTreeCallback->text().toUtf8().constData();
}


std::string CallbacksDialog::getVoidCallback() {
    return _dialog.lineEditVoidCallback->text().toUtf8().constData();
}


bool CallbacksDialog::runInfoOnSelection() {
    return _dialog.checkBoxRunInfoOnSelection->isChecked();
}


bool CallbacksDialog::runTreeOnSelection() {
    return _dialog.checkBoxRunTreeOnSelection->isChecked();
}


bool CallbacksDialog::runVoidOnSelection() {
    return _dialog.checkBoxRunVoidOnSelection->isChecked();
}
