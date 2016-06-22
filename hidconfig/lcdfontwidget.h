#ifndef LCDFONTWIDGET_H
#define LCDFONTWIDGET_H

#include <QWidget>

class LCDFontWidget : public QWidget
{
    Q_OBJECT

public:
    LCDFontWidget(QWidget *parent = NULL );
    ~LCDFontWidget();

    void setSize( int nRows, int nCols );
    void setData( const QVector<byte> &data );
    const QVector<byte> &data() const { return m_data; }

    void clearFont();

private:
    virtual void paintEvent( QPaintEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual int heightForWidth ( int w ) const;
    virtual QSize sizeHint() const;
    virtual void mousePressEvent( QMouseEvent * event );

    QRect CellQRect( int nRow, int nCol );
    void InvalidateCell( int nRow, int nCol );
    void setCell( int r, int c, bool bSet );
    bool cellHit( QPointF pos, int &r, int &c );

    int m_nRows;
    int m_nCols;
    QSize m_sizeArea;
    QSize m_sizeSmallArea;;
    QImage m_imgLCD;
    QBrush m_brushForeground;
    QBrush m_brushBackground;

    QVector<byte> m_data;
};

#endif // LCDFONTWIDGET_H
