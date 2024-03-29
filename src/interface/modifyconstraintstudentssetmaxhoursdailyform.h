/***************************************************************************
                          modifyconstraintstudentssetmaxhoursdailyform.h  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXHOURSDAILYFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXHOURSDAILYFORM_H

#include "ui_modifyconstraintstudentssetmaxhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxHoursDailyForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxHoursDaily* _ctr;

	ModifyConstraintStudentsSetMaxHoursDailyForm(QWidget* parent, ConstraintStudentsSetMaxHoursDaily* ctr);
	~ModifyConstraintStudentsSetMaxHoursDailyForm();

	void updateStudentsComboBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
