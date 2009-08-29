// hidcomp/hidconfig, HID device interface for emc
// Copyright (C) 2008, Frank Tkalcevic, www.franksworkshop.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <map>

#include <assert.h>

#ifdef _WIN32
#pragma warning(push, 1)
#endif
#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QMenu>
#include <QSlider>
#include <QAction>
#include <QSpinBox>

#include <QString>
#include <QTextStream>
#include <QtGui/QMainWindow>
#include <QThread>
#include <QReadLocker>
#include <QWriteLocker>
#include <QReadWriteLock>
#include <QVector>
#include <QDomDocument>
#include <QFileDialog>
#include <QMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QPainter>
#include <QMouseEvent>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QCloseEvent>
#include <QList>
#include <QVector>
#include <QSettings>

#ifdef _WIN32
#pragma warning(pop)
#pragma warning(disable:4251)
#endif

#include "utility.h"
#include "datatypes.h"
#include "hidtypes.h"
#include "log.h"

#include "common.h"


#ifndef _WIN32
#define _snprintf snprintf
#endif
#endif
