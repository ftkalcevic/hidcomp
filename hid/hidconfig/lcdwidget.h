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
