/*
 find_outline_by_name_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "find_outline_by_name_dialog.h"

namespace m8r {

using namespace std;

FindOutlineByNameDialog::FindOutlineByNameDialog(QWidget *parent)
    : QDialog(parent)
{
    // widgets
    label = new QLabel{tr("Outline name:")};
    lineEdit = new QLineEdit{this};
    label->setBuddy(lineEdit);

    listView = new QListView(this);
    listView->setModel(&listViewModel);

    caseCheckBox = new QCheckBox{tr("&Ignore case")};
    caseCheckBox->setChecked(true);

    findButton = new QPushButton{tr("&Open Outline")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString&)));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(handleReturn()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout *mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addWidget(listView);
    mainLayout->addWidget(caseCheckBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Find Outline by Name"));
    // height is set to make sure listview gets enough lines
    resize(fontMetrics().averageCharWidth()*50, fontMetrics().height()*30);
    setModal(true);
}

void FindOutlineByNameDialog::enableFindButton(const QString& text)
{
    // filter listview
    // IMPROVE use HSTR algorithm to be smarter
    filteredListViewStrings.clear();
    if(!text.isEmpty()) {
        Qt::CaseSensitivity c = caseCheckBox->isChecked()?Qt::CaseInsensitive:Qt::CaseSensitive;
        for(QString& n:listViewStrings) {
            if(n.startsWith(text,c)) {
                filteredListViewStrings << n;
            }
        }
    } else {
        filteredListViewStrings << listViewStrings;
    }
    ((QStringListModel*)listView->model())->setStringList(filteredListViewStrings);

    findButton->setEnabled(!filteredListViewStrings.isEmpty());
}

void FindOutlineByNameDialog::show(vector<string>* outlineNames)
{
    listViewStrings.clear();
    filteredListViewStrings.clear();
    if(outlineNames && outlineNames->size()) {
        for(string& n:*outlineNames) {
            listViewStrings << QString::fromStdString(n);
        }
        listViewStrings.sort();
        filteredListViewStrings << listViewStrings;
        ((QStringListModel*)listView->model())->setStringList(filteredListViewStrings);
    }

    findButton->setEnabled(!listViewStrings.isEmpty());
    lineEdit->clear();
    lineEdit->setFocus();

    QDialog::show();
}

void FindOutlineByNameDialog::handleReturn(void)
{
    if(findButton->isEnabled()) {
        choice = filteredListViewStrings.at(0);

        QDialog::close();
        emit searchFinished();
    }
}

void FindOutlineByNameDialog::handleChoice(void)
{
    if(listView->currentIndex().isValid()) {
        choice = filteredListViewStrings.at(listView->currentIndex().row());

        QDialog::close();
        emit searchFinished();
    }
}

FindOutlineByNameDialog::~FindOutlineByNameDialog()
{
    delete label;
    delete lineEdit;
    delete listView;
    delete caseCheckBox;
    delete findButton;
    delete closeButton;
}

}