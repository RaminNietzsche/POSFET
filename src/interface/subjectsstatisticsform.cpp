/***************************************************************************
                          subjectsstatisticform.cpp  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#include "subjectsstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include <QString>
#include <QStringList>

#include <QTableWidget>
#include <QHeaderView>

SubjectsStatisticsForm::SubjectsStatisticsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);
	
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
		
	tableWidget->clear();
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(gt.rules.subjectsList.size());
	
	QStringList columns;
	columns<<tr("Subject");
	columns<<tr("No. of activities");
	columns<<tr("Duration");
	
	tableWidget->setHorizontalHeaderLabels(columns);
	
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		
		QTableWidgetItem* newItem=new QTableWidgetItem(s->name);
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		tableWidget->setItem(i, 0, newItem);

		int	nSubActivities=0;
		int nHours=0;
		
		foreach(Activity* act, gt.rules.activitiesList)
			if(act->active)
				if(act->subjectName==s->name){
					nSubActivities++;
					nHours+=act->duration;
				}

		newItem=new QTableWidgetItem(CustomFETString::number(nSubActivities));
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		tableWidget->setItem(i, 1, newItem);

		newItem=new QTableWidgetItem(CustomFETString::number(nHours));
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		tableWidget->setItem(i, 2, newItem);
	}

	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}

SubjectsStatisticsForm::~SubjectsStatisticsForm()
{
	saveFETDialogGeometry(this);
}
