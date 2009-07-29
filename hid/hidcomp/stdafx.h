#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <stdio.h>


#ifdef _WIN32
#pragma warning(push, 1)
#endif

#include <QMap>
#include <QString>
#include <QRegExp>
#include <QTime>
#include <QVarLengthArray>
#include <QCoreApplication>

#ifdef _WIN32
#pragma warning(pop)
#pragma warning(disable:4251)
#endif

#include "common.h"
#include "log.h"


#endif
