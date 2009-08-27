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

#ifndef LCDWIDGET_H
#define LCDWIDGET_H

#include <QWidget>
#include "lcdchar.h"

class LCDWidget : public QWidget
{
    Q_OBJECT

public:
    LCDWidget(QWidget *parent);
    ~LCDWidget();
    void Write( int nRow, int nCol, const QString &s, bool bHighlight );
    void SetUserFont( byte index, const QVector<byte> &data );
    void Erase( int nRow, int nCol, int nLen );
    void Clear();
    void setSize( int nRows, int nCols );

private:
    virtual void paintEvent( QPaintEvent * evnt );
    virtual void resizeEvent ( QResizeEvent * evnt );
    virtual int heightForWidth ( int w ) const;
    virtual QSize sizeHint() const;

    void InvalidateCells( int nRow, int nCol, int n );
    QRect CellQRect( int nRow, int nCol );

    QSize m_sizeTextArea;
    QImage m_imgLCD;

    QBrush m_bkgBrush;
    static QMap<int,LCDChar*> m_chars;
    int m_nRows;
    int m_nCols;
};

#endif // LCDWIDGET_H
