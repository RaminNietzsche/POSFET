/***************************************************************************
                          constraintactivitypreferredroomform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "constraintactivitypreferredroomform.h"
#include "addconstraintactivitypreferredroomform.h"
#include "modifyconstraintactivitypreferredroomform.h"

#include "lockunlock.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintActivityPreferredRoomForm::ConstraintActivityPreferredRoomForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);
	
	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(constraintChanged(int)));
	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeConstraintPushButton, SIGNAL(clicked()), this, SLOT(removeConstraint()));
	connect(modifyConstraintPushButton, SIGNAL(clicked()), this, SLOT(modifyConstraint()));
	connect(roomsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyConstraint()));
	connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	roomsComboBox->addItem("");
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		roomsComboBox->addItem(rm->name);
	}

/////////////
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
	teachersComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
	}
	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
	}
	activityTagsComboBox->setCurrentIndex(0);

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
	studentsComboBox->setCurrentIndex(0);
///////////////

	this->filterChanged();
}

ConstraintActivityPreferredRoomForm::~ConstraintActivityPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityPreferredRoomForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
		return false;
		
	ConstraintActivityPreferredRoom* c=(ConstraintActivityPreferredRoom*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
		
	bool found=true;
	
	int id=c->activityId;
	Activity* act=NULL;
	foreach(Activity* a, gt.rules.activitiesList)
		if(a->id==id)
			act=a;

	found=true;
		
	if(act!=NULL){
		//teacher
		if(tn!=""){
			bool ok2=false;
			for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
				if(*it == tn){
					ok2=true;
					break;
				}
			if(!ok2)
				found=false;
		}

		//subject
		if(sbn!="" && sbn!=act->subjectName)
			found=false;
	
		//activity tag
		if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
			found=false;
	
		//students
		if(stn!=""){
			bool ok2=false;
			for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
				if(*it == stn){
					ok2=true;
					break;
			}
			if(!ok2)
				found=false;
		}
	}
	
	if(!found)
		return false;
	
	return c->roomName==roomsComboBox->currentText() || roomsComboBox->currentText()=="";
}

void ConstraintActivityPreferredRoomForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		}
	}
	
	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		constraintChanged(-1);
}

void ConstraintActivityPreferredRoomForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setPlainText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintActivityPreferredRoomForm::addConstraint()
{
	AddConstraintActivityPreferredRoomForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintActivityPreferredRoomForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivityPreferredRoomForm form(this, (ConstraintActivityPreferredRoom*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintActivityPreferredRoomForm::removeConstraint()
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
		
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
		
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
