// Copyright (c) 2014, Justin Eldridge, Mikhail Belkin, and Yusu Wang
// at The Ohio State University. All rights reserved.
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
