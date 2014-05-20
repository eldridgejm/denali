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


void ChooseRootDialog::setMinimumNodeChecked(bool option) const {
    _dialog.radioButtonMinimumNode->setChecked(option);
}


void ChooseRootDialog::setMaximumNodeChecked(bool option) const {
    _dialog.radioButtonMaximumNode->setChecked(option);
}
