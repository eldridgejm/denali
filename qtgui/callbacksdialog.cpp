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

    connect(_dialog.pushButtonBrowseAsync, SIGNAL(clicked()),
            this, SLOT(setAsyncCallback()));

    connect(_dialog.pushButtonClearInfo, SIGNAL(clicked()),
            this, SLOT(clearInfoCallback()));

    connect(_dialog.pushButtonClearTree, SIGNAL(clicked()),
            this, SLOT(clearTreeCallback()));

    connect(_dialog.pushButtonClearAsync, SIGNAL(clicked()),
            this, SLOT(clearAsyncCallback()));
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


void CallbacksDialog::setAsyncCallback()
{
    // open a file dialog to get the filename
    QString qfilename = QFileDialog::getOpenFileName(
            this, tr("Select Async Callback"), "", tr("Files(*)"));

    // convert the filename to a std::string
    std::string filename = qfilename.toUtf8().constData();

    _dialog.lineEditAsyncCallback->setText(qfilename);
}


std::string CallbacksDialog::getInfoCallback() {
    return _dialog.lineEditInfoCallback->text().toUtf8().constData();
}


std::string CallbacksDialog::getTreeCallback() {
    return _dialog.lineEditTreeCallback->text().toUtf8().constData();
}


std::string CallbacksDialog::getAsyncCallback() {
    return _dialog.lineEditAsyncCallback->text().toUtf8().constData();
}


bool CallbacksDialog::runInfoOnSelection() {
    return _dialog.checkBoxRunInfoOnSelection->isChecked();
}


bool CallbacksDialog::runTreeOnSelection() {
    return _dialog.checkBoxRunTreeOnSelection->isChecked();
}


bool CallbacksDialog::runAsyncOnSelection() {
    return _dialog.checkBoxRunAsyncOnSelection->isChecked();
}


bool CallbacksDialog::provideInfoSubtree() {
    return _dialog.checkBoxInfoSubtree->isChecked();
}


bool CallbacksDialog::provideTreeSubtree() {
    return _dialog.checkBoxTreeSubtree->isChecked();
}


bool CallbacksDialog::provideAsyncSubtree() {
    return _dialog.checkBoxAsyncSubtree->isChecked();
}


void CallbacksDialog::clearInfoCallback() {
    _dialog.lineEditInfoCallback->clear();
    _dialog.checkBoxRunInfoOnSelection->setChecked(false);
    _dialog.checkBoxInfoSubtree->setChecked(false);
}


void CallbacksDialog::clearTreeCallback() {
    _dialog.lineEditTreeCallback->clear();
    _dialog.checkBoxRunTreeOnSelection->setChecked(false);
    _dialog.checkBoxTreeSubtree->setChecked(false);
}


void CallbacksDialog::clearAsyncCallback() {
    _dialog.lineEditAsyncCallback->clear();
    _dialog.checkBoxRunAsyncOnSelection->setChecked(false);
    _dialog.checkBoxAsyncSubtree->setChecked(false);
}
