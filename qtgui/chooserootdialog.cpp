#include "chooserootdialog.h"

#include <QtGui>
#include <QIntValidator>

ChooseRootDialog::ChooseRootDialog(QWidget* parent) :
        QDialog(parent) 
{
    _dialog.setupUi(this);

    // make sure that only positive integers can be entered for a node
    QIntValidator* node_validator = new QIntValidator(this);
    node_validator->setBottom(0);
    _dialog.lineEditOtherNode->setValidator(node_validator);
}


bool ChooseRootDialog::isMinimumNodeChecked() const {
    return _dialog.radioButtonMinimumNode->isChecked();
}


bool ChooseRootDialog::isMaximumNodeChecked() const {
    return _dialog.radioButtonMaximumNode->isChecked();
}


bool ChooseRootDialog::isOtherNodeChecked() const {
    return _dialog.radioButtonOtherNode->isChecked();
}


unsigned int ChooseRootDialog::getOtherNode() const {
    
    return _dialog.lineEditOtherNode->text().toInt();
}
