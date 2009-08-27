#include "stdafx.h"
#include "lcdfontwidget.h"
#include "hid.h"

#define PIXEL_SIZE  10
#define X_BORDER    2
#define X_SPACING   2
#define Y_BORDER    2
#define Y_SPACING   2

LCDFontWidget::LCDFontWidget(QWidget *parent)
: QWidget(parent)
{
    m_nRows = 7;
    m_nCols = 5;
    m_data.resize( m_nCols );

    setSize( m_nRows, m_nCols );

    QSizePolicy policy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    policy.setHeightForWidth( true );
    setSizePolicy( policy );

    m_brushBackground = QBrush(QColor(134, 154, 58));
    m_brushForeground = QBrush(Qt::black);

    clearFont();
}

LCDFontWidget::~LCDFontWidget()
{
}

void LCDFontWidget::setSize( int nRows, int nCols )
{
    m_nRows = nRows;
    m_nCols = nCols;

    m_sizeArea = QSize( 2*X_BORDER + m_nCols * (PIXEL_SIZE+X_SPACING)-X_SPACING, 2*Y_BORDER + m_nRows * (PIXEL_SIZE + Y_SPACING) - Y_SPACING );
    m_sizeSmallArea = QSize( 2*X_BORDER + m_nCols * 2, 2*Y_BORDER + m_nRows * 2 );
    m_imgLCD = QImage(m_sizeArea, QImage::Format_ARGB32 );
}


void LCDFontWidget::setData( const QVector<byte> &data )
{
    assert( data.size() == m_nCols );

    if ( data.size() != m_nCols )
	return;

    clearFont();
    m_data.resize( data.size() );
    for ( int i = 0; i < data.count(); i++ )
	m_data[i] = data[i];

    for ( int r = 0; r < m_nRows; r++ )
	for ( int c = 0; c < m_nCols; c++ )
	    setCell( r, c, LCDFont::GetFontBit( m_nCols, r, c, m_data.data() ) );
}


void LCDFontWidget::paintEvent( QPaintEvent * )
{
    QSize sz = size();
    sz.setWidth( sz.width() - m_sizeSmallArea.width() );

    QImage img = m_imgLCD.scaled( sz, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    QPainter p2( this );
    p2.setRenderHint( QPainter::HighQualityAntialiasing, true );
    p2.setRenderHint( QPainter::SmoothPixmapTransform, true );
    p2.drawImage( QPoint(0,0), img );

    QImage imgSmall = m_imgLCD.scaled( m_sizeSmallArea, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    p2.drawImage( QPoint(img.width() + X_SPACING,img.height()-imgSmall.height()), imgSmall );
}

void LCDFontWidget::clearFont()
{
    m_imgLCD.fill( m_brushBackground.color().rgba() );
    for ( int i = 0; i < m_nCols; i++ )
	m_data[i] = 0;
    update();
}

void LCDFontWidget::resizeEvent( QResizeEvent * )
{
    setGeometry( x(), y(), width(), heightForWidth(width()) );
    updateGeometry();
}

int LCDFontWidget::heightForWidth ( int w ) const
{
    w -= m_sizeSmallArea.width();
    double h = (double)w * (double)(m_sizeArea.height()) / (double)(m_sizeArea.width());
    return (int)h;
}

QSize LCDFontWidget::sizeHint() const
{
    int w = width();
    QSize s = QSize( w, heightForWidth(w) );
    return s;;
}

// handle mouse press event to draw pixels
void LCDFontWidget::mousePressEvent( QMouseEvent * event )
{
    int r, c;
    if ( cellHit( event->posF(), r, c ) )
    {
	if ( event->button() == Qt::LeftButton )
	    setCell( r, c, true );
	else if ( event->button() == Qt::RightButton )
	    setCell( r, c, false );
    }
}

// Find the font pixel under the cusor pos and return the row, column
bool LCDFontWidget::cellHit( QPointF pos, int &r, int &c )
{
    QRectF rc(QPointF(0,0),QSizeF(size().width()-m_sizeSmallArea.width(),size().height()));
    if ( rc.contains(pos) )
    {
	double scale_x = (double)rc.width() / (double)m_sizeArea.width();
	double scale = (double)rc.height() / (double)m_sizeArea.height();
	if ( scale < scale_x )
	    scale = scale_x;

	c = (int)((pos.x() / scale - X_BORDER) / (PIXEL_SIZE + X_SPACING));
	r = (int)((pos.y() / scale - Y_BORDER) / (PIXEL_SIZE + Y_SPACING));
	if ( r >= 0 && r < m_nRows &&
	     c >= 0 && c < m_nCols )
	    return true;
    }
    return false;
}

// Set/clear a cell
void LCDFontWidget::setCell( int r, int c, bool bSet )
{
    QPainter p( &m_imgLCD );
    QRect rc = CellQRect( r, c );
    p.fillRect( rc, bSet ? m_brushForeground : m_brushBackground ); 

    InvalidateCell( r, c );
    LCDFont::SetFontBit( m_nCols, r, c, m_data.data(), bSet );
}

// Force repaint of a cell
void LCDFontWidget::InvalidateCell( int /*nRow*/, int /*nCol*/ )
{
//  Map RC cell coordinates to screen coordinates
    update();
}

// Return the cell bitmap rectangle
QRect LCDFontWidget::CellQRect( int nRow, int nCol )
{
    return QRect( QPoint( X_BORDER + nCol * (PIXEL_SIZE+X_SPACING), Y_BORDER + nRow * (PIXEL_SIZE+Y_SPACING) ),
                  QSize( PIXEL_SIZE, PIXEL_SIZE ) );
}


