/***************************************************************************
                          constraintstudentssethomeroomsform.cpp  -  description
                             -------------------
    begin                : 8 April 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "constraintstudentssethomeroomsform.h"
#include "addconstraintstudentssethomeroomsform.h"
#include "modifyconstraintstudentssethomeroomsform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintStudentsSetHomeRoomsForm::ConstraintStudentsSetHomeRoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(removeConstraintPushButton, SIGNAL(clicked()), this, SLOT(removeConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(constraintChanged(int)));
	connect(modifyConstraintPushButton, SIGNAL(clicked()), this, SLOT(modifyConstraint()));
	connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyConstraint()));
	connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(roomsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	studentsComboBox->addItem("");

	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->addItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->addItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->addItem(sts->name);
			}
		}
	}
	
	roomsComboBox->addItem("");
	foreach(Room* rm, gt.rules.roomsList)
		roomsComboBox->addItem(rm->name);

	this->refreshConstraintsListWidget();
}

ConstraintStudentsSetHomeRoomsForm::~ConstraintStudentsSetHomeRoomsForm()
{
	saveFETDialogGeometry(this);
}

void ConstraintStudentsSetHomeRoomsForm::refreshConstraintsListWidget()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
		if(filterOk(ctr)){
			QString s;
			s=ctr->getDescription(gt.rules);
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(s);
		}
	}

	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		constraintsListWidget->setCurrentRow(-1);
}

void ConstraintStudentsSetHomeRoomsForm::filterChanged()
{
	this->refreshConstraintsListWidget();
}

bool ConstraintStudentsSetHomeRoomsForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
		ConstraintStudentsSetHomeRooms* c=(ConstraintStudentsSetHomeRooms*)ctr;
		return (c->studentsName==studentsComboBox->currentText() || studentsComboBox->currentText()=="")
		  && (roomsComboBox->currentText()=="" || c->roomsNames.contains(roomsComboBox->currentText()));
	}
	else
		return false;
}

void ConstraintStudentsSetHomeRoomsForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	QString s;
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void ConstraintStudentsSetHomeRoomsForm::addConstraint()
{
	AddConstraintStudentsSetHomeRoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	this->refreshConstraintsListWidget();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintStudentsSetHomeRoomsForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsSetHomeRoomsForm form(this, (ConstraintStudentsSetHomeRooms*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	this->refreshConstraintsListWidget();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintStudentsSetHomeRoomsForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);

	QListWidgetItem* item;
	
	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		gt.rules.removeSpaceConstraint(ctr);
		
		visibleConstraintsList.removeAt(i);
		constraintsListWidget->setCurrentRow(-1);
		item=constraintsListWidget->takeItem(i);
		delete item;

		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}
