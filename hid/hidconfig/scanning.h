#ifndef SCANNING_H
#define SCANNING_H

#include "ui_scanning.h"

class scanning : public QWidget
{
	Q_OBJECT

public:
	scanning(QWidget *parent = 0);
	~scanning();

private:
	Ui::scanningClass ui;
};

#endif // SCANNING_H
