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

#include "stdafx.h"
#include "controlpointswidget.h"




ControlPointsWidget::ControlPointsWidget(QWidget *parent)
: QWidget(parent),
m_bChanged( false ),
m_bInit( false ),
m_bHasFocus( false ),
m_bHasWorkingData( false ),
m_bSnap( false ),
m_bDragging( false ),
m_nValue(0),
m_nFocusPoint(-1),
m_nDeviceMin(0),
m_nDeviceMax(1024),
m_bReverse( false ),
m_pPopupMenu( NULL )
{
    setMouseTracking( true );
    setFocusPolicy( Qt::WheelFocus );

    m_pPopupMenu = new QMenu("Edit Nodes");
    m_pAdd = m_pPopupMenu->addAction( "Add Node" );
    m_pRemove = m_pPopupMenu->addAction( "Remove Node" );
}

ControlPointsWidget::~ControlPointsWidget()
{

}


void ControlPointsWidget::paintEvent( QPaintEvent * )
{
    // Background

    QPainter p(this);

    QRect rc = rect();

    if ( !m_bInit )
    {
	m_bInit = true;
	int nSpacing = 3;

	// Remove 10% for the test bar
	m_rcChart = rc.adjusted( nSpacing*2, nSpacing*2, -(nSpacing * 6 + rc.width()/10), -nSpacing * 2 );

	m_rcChartPlotArea = m_rcChart.adjusted( 5,5,-5,-5 );

	m_rcChartHorzCentre = m_rcChart;
	m_rcChartHorzCentre.setTop( m_rcChartHorzCentre.top() + m_rcChart.height()/2 - m_rcChart.height()/10 );
	m_rcChartHorzCentre.setBottom( m_rcChartHorzCentre.top() + m_rcChart.height()/5 );

	m_rcChartVertCentre = m_rcChart;
	m_rcChartVertCentre.setLeft(m_rcChartVertCentre.left() + m_rcChart.width()/2 - m_rcChart.width()/10 );
	m_rcChartVertCentre.setRight( m_rcChartVertCentre.left() + m_rcChart.width()/5 );

	m_rcTestBar = m_rcChart;
	m_rcTestBar.setLeft( m_rcChart.right() + nSpacing );
	m_rcTestBar.setRight( rc.right() - nSpacing );

	m_rcTestBarText = p.fontMetrics().boundingRect("100%");
	m_rcTestBarText.adjust( -4, -4, 4, 4 );
	m_rcTestBarText.moveCenter( m_rcTestBar.center() );

	m_IndicatorColour = QColor(255,0,0);
    }

    p.fillRect( rc, palette().brush(QPalette::Base) );

    p.fillRect( m_rcChart, palette().brush(QPalette::AlternateBase) );
    p.fillRect( m_rcTestBar, palette().brush(QPalette::AlternateBase) );
    p.fillRect( m_rcChartHorzCentre, palette().brush(QPalette::Base) );
    p.fillRect( m_rcChartVertCentre, palette().brush(QPalette::Base) );
    if ( m_bHasFocus )
    {
	p.setBrush( Qt::NoBrush );
	p.drawRect( rc.adjusted(0,0,-1,-1) );
    }

    // Draw the grid.
    if ( !m_bHasWorkingData )
	CalculateWorkingData();

    // Draw the vertical line in the left part of the chart to show the hid device position
    int lP = m_nValue;
    lP = ScaleX(lP);
    p.setPen(m_IndicatorColour);
    p.drawLine( lP, m_rcChart.top(), lP, m_rcChart.bottom() );

    // show the hid device position as a bar chart
    QRect rcBarIndicator = m_rcTestBar;
    rcBarIndicator.setTop( ScaleY( Interpolate( m_nValue ) ) );
    p.fillRect( rcBarIndicator, m_IndicatorColour );
    // and text
    int nPercent = Interpolate( m_nValue )/100;
    p.fillRect( m_rcTestBarText, palette().brush(QPalette::Base) );
    p.drawText( m_rcTestBarText, Qt::AlignHCenter | Qt::AlignVCenter, tr("%1%").arg(nPercent) );

    // draw the line
    int old_x=0;
    int old_y=0;
    for ( int i = 0; i < (int)m_Points.size(); i++ )
    {
	int x = ScaleX(m_Points[i].lP);
	int y = ScaleY(m_Points[i].dwLog);

	if ( m_bDragging && m_nFocusPoint == i )
	{
	    //x += m_ptDragOffset.x;
	    //y += m_ptDragOffset.y;
	    //x = SnapScreenX(x);
	    //y = SnapY(y);
	    //TRACE(" snap (%d,%d)\n", x,y );
	    x = ScaleX(m_ptDragCPoint.x());
	    y = ScaleY(m_ptDragCPoint.y());
	}

	if ( i > 0 )
	    p.drawLine(old_x, old_y, x, y );
	p.fillRect( x-3, y-3, 7, 7, QColor(0,0,0) );

	old_x = x;
	old_y = y;
    }
}


void ControlPointsWidget::CalculateWorkingData()
{
    m_bHasWorkingData = true;
    m_dScaleX = ((double)m_rcChartPlotArea.width()) / POINTS_MAX;
    m_dScaleY = ((double)m_rcChartPlotArea.height()) / POINTS_MAX;

    for ( int i = 0; i < (int)m_Points.size(); i++ )
    {
	int x = ScaleX( m_Points[i].lP );
	int y = ScaleY( m_Points[i].dwLog );

	m_rcScreenPoints[i] = QRect( QPoint(x-3,y-3), QSize(7,7));
    }
}


void ControlPointsWidget::setPoints( std::vector<CPoint> &points )
{
    m_Points.clear();
    m_Points = points;
    m_rcScreenPoints.clear();
    m_rcScreenPoints.resize( m_Points.size() );
    m_bHasWorkingData = false;
    m_bChanged = false;
    update();
}


// Convert cpoint to screen point
int ControlPointsWidget::ScaleX( int x )
{
    return (int)((double)m_rcChartPlotArea.left() + (double)x * m_dScaleX);
}

// Convert cpoint to screen point
int ControlPointsWidget::ScaleY( int y )
{
    return (int)((double)m_rcChartPlotArea.bottom() - (double)(y*m_dScaleY) );
}

// Convert screen point to cpoint
int ControlPointsWidget::UnscaleX( int x )
{
    return (int)( (double)(x - m_rcChartPlotArea.left()) / m_dScaleX);
}

// Convert screen point to cpoint
int ControlPointsWidget::UnscaleY( int y )
{
    return (int)( (double)(m_rcChartPlotArea.bottom() - y) / m_dScaleY);
}

int ControlPointsWidget::Interpolate( int lP )
{
    // interpolate the logical value from the raw value in screen coordinates.
    int x;
    for ( int i = 0; i < (int)m_Points.size(); i++ )
    {
	x = m_Points[i].lP;

	if ( lP < x )
	{
	    if ( i == 0 )
		return m_Points[0].dwLog;
	    else
	    {
		int x0 = m_Points[i-1].lP;
		int y0 = m_Points[i-1].dwLog;
		int y = m_Points[i].dwLog;

		if ( x == x0 )
		    return y;

		int ret = (lP - x0) * ( y - y0 ) / ( x - x0) + y0;

		return ret;
	    }
	}
    }

    return m_Points.rbegin()->dwLog;
}

// set snap - input cpoint
int ControlPointsWidget::SnapX( int lP )
{
    if ( !m_bSnap )
	return lP;
    else
    {
	// snap at 2.5%
	int step = POINTS_MAX/40;
	lP = ((lP + step/2)/ step) * step;
	return lP;
    }
}

// set snap - input cpoint
int ControlPointsWidget::SnapY( int logY )
{
    if ( !m_bSnap )
	return logY;
    else
    {
	// snap at 5%
	int step = POINTS_MAX/20;
	logY = ((logY + step/2)/step) * step;
	return logY;
    }
}



void ControlPointsWidget::focusInEvent(QFocusEvent *)
{
    m_bHasFocus = true;
    m_nValue = -1;
    update();
}

void ControlPointsWidget::focusOutEvent(QFocusEvent *)
{
    m_bHasFocus = false;
    update();
}


void ControlPointsWidget::mousePressEvent ( QMouseEvent * evnt )
{
    if ( evnt->button() == Qt::LeftButton )
    {
	if ( m_nFocusPoint >= 0 )	// Have we "mousemoved" over a point
	{
	    setCursor( Qt::ClosedHandCursor );

	    m_bDragging = true;
	    //m_ptDragPickup = pt;
	    //m_ptDragOffset.SetPoint(0,0);
	    m_ptDragCPoint.setX( SnapX(UnscaleX(evnt->x())) );
	    m_ptDragCPoint.setY( SnapY(UnscaleY(evnt->y())) );
	    //SetCapture();
	}
    }
    else if ( evnt->button() == Qt::RightButton && !m_bDragging )
    {
	if ( m_nFocusPoint >= 0 )
	    m_pRemove->setEnabled( true );
	else
	    m_pRemove->setEnabled( false );
	QAction *pAction = m_pPopupMenu->exec( QCursor::pos() );
	if ( pAction == m_pAdd )
	{
	    int index;
	    if ( m_nFocusPoint == (int)m_Points.size() - 1 )
		index = m_Points.size() - 2;
	    else if ( m_nFocusPoint >= 0 )
		index = m_nFocusPoint;
	    else
	    {
		int x = UnscaleX(evnt->x());
		int y = UnscaleY(evnt->y());

		// find the closest points
		double min_diff = 1E20;
		int index2=0;
		double min2_diff = 1E20;
		index=0;
		for ( int i = 0; i < (int)m_Points.size(); i++ )
		{
		    double diff = (double)(m_Points[i].lP - x) * (double)(m_Points[i].lP - x) + (double)(m_Points[i].dwLog - y)*(double)(m_Points[i].dwLog - y);
		    if ( diff < min_diff )
		    {
			min2_diff = min_diff;
			index2 = index;
			min_diff = diff;
			index = i;
		    }
		    else if ( diff < min2_diff )
		    {
			min2_diff = diff;
			index2 = i;
		    }
		}

		if ( index2 < index )
		    index = index2;
	    }

	    CPoint pt( (m_Points[index].lP + m_Points[index+1].lP)/2, (m_Points[index].dwLog + m_Points[index+1].dwLog)/2 );
	    m_Points.insert( m_Points.begin() + index + 1, pt );
	    m_bChanged = true;
	    m_rcScreenPoints.resize(m_rcScreenPoints.size()+1);	// just add any screen point - all will be recalculated.
	    CalculateWorkingData();
	    update();
	}
	else if ( m_nFocusPoint >= 0 )
	{
	    if ( m_Points.size() > 2 )
	    {
		m_Points.erase( m_Points.begin() + m_nFocusPoint );
		m_bChanged = true;
		m_rcScreenPoints.pop_back();	// just remove any screen point - all will be recalculated.
		CalculateWorkingData();
		update();
	    }
	}
    }
}

void ControlPointsWidget::mouseReleaseEvent( QMouseEvent * evnt )
{
    if ( evnt->button() == Qt::LeftButton )
    {
	if ( m_bDragging )
	{
	    QRect rcBounds = m_rcChartPlotArea.adjusted(0,0,1,1);
	    QPoint pt = ClipPoint( rcBounds, evnt->pos() );

	    // Convert screen coordinates back to CPOINT coordinates.
	    m_Points[m_nFocusPoint].lP = SnapX(UnscaleX(pt.x()));
	    m_Points[m_nFocusPoint].dwLog = SnapY(UnscaleY(pt.y()));
	    m_bChanged = true;

	    CalculateWorkingData();
	    update();
	    m_bDragging = false;
	    SetMouseCursor( evnt->pos() );
	    //ReleaseCapture();
	}
    }
}

// set the mouse cursor based on what we are over.  Only called when not dragging.
void ControlPointsWidget::SetMouseCursor( const QPoint &pos )
{
    bool bSet = false;
    for ( int i = 0; i < (int)m_rcScreenPoints.size(); i++ )
	if ( m_rcScreenPoints[i].contains(pos) )
	{
	    m_nFocusPoint = i;
	    setCursor( Qt::OpenHandCursor );
	    bSet = true;
	    break;
	}

	if ( !bSet )
	{
	    setCursor( Qt::ArrowCursor );
	    m_nFocusPoint = -1;
	}
}

void ControlPointsWidget::mouseMoveEvent ( QMouseEvent * evnt )
{
    QPoint pt = evnt->pos();

    if ( !m_bDragging )
    {
	SetMouseCursor( pt );
    }
    else
    {
	QRect rcBounds = m_rcChartPlotArea.adjusted(0,0,1,1);
	if ( !rcBounds.contains(pt) )
	{
	    // Cursor trying to move out of area
	    pt = ClipPoint( rcBounds, pt );
	}

	// Invalidate the rectangle that bounds the old point and joining line segments.
	//int x = ScaleX(m_Points[m_nFocusPoint].lP);
	//int y = ScaleY(m_Points[m_nFocusPoint].dwLog);
	//x += m_ptDragOffset.x;
	//y += m_ptDragOffset.y;
	//x = SnapScreenX(x);
	//y = SnapY(y);
	int x = ScaleX(m_ptDragCPoint.x());
	int y = ScaleY(m_ptDragCPoint.y());

	QRect rcInvalid;
	if ( m_nFocusPoint == 0 )
	{
	    int x1 = ScaleX(m_Points[1].lP);
	    int y1 = ScaleY(m_Points[1].dwLog);
	    QRect rc1(x-3,y-3,x+3,y+3 );
	    QRect rc2(x1-3,y1-3,x1+3,y1+3 );

	    rc1 = rc1.normalized();
	    rc2 = rc2.normalized();

	    rc1.adjust(-1,-1,1,1);
	    rc2.adjust(-1,-1,1,1);
	    rcInvalid = rc1.united( rc2 );
	}
	else if ( m_nFocusPoint == (int)m_Points.size()-1 )
	{
	    int x1 = ScaleX(m_Points[m_nFocusPoint-1].lP);
	    int y1 = ScaleY(m_Points[m_nFocusPoint-1].dwLog);

	    QRect rc1(x-3,y-3,x+3,y+3 );
	    QRect rc2(x1-3,y1-3,x1+3,y1+3 );

	    rc1 = rc1.normalized();
	    rc2 = rc2.normalized();

	    rc1.adjust(-1,-1,1,1);
	    rc2.adjust(-1,-1,1,1);
	    rcInvalid = rc1.united( rc2 );
	}
	else
	{
	    QRect rc1(x-3,y-3,x+3,y+3 );
	    rc1 = rc1.normalized();
	    rc1.adjust(-1,-1,1,1);

	    int x1 = ScaleX(m_Points[m_nFocusPoint-1].lP);
	    int y1 = ScaleY(m_Points[m_nFocusPoint-1].dwLog);
	    QRect rc2(x1-3,y1-3,x1+3,y1+3 );
	    rc2 = rc2.normalized();
	    rc2.adjust(-1,-1,1,1);

	    x1 = ScaleX(m_Points[m_nFocusPoint+1].lP);
	    y1 = ScaleY(m_Points[m_nFocusPoint+1].dwLog);
	    QRect rc3(x1-3,y1-3,x1+3,y1+3 );
	    rc3 = rc3.normalized();
	    rc3.adjust(-1,-1,1,1);

	    QRect temp;
	    temp = rc1.united(rc2);
	    rcInvalid = temp.united( rc3 );
	}

	// Update the drag offset.  This will be repainted in the paint routine.
	//m_ptDragOffset = pt - m_ptDragPickup;
	m_ptDragCPoint.setX( SnapX(UnscaleX(pt.x())) );
	m_ptDragCPoint.setY( SnapY(UnscaleY(pt.y())) );
	update( rcInvalid );
    }

    //CRect rcBounds(m_rcChartPlotArea.left, m_rcChartPlotArea.top, m_rcChartPlotArea.right+1, m_rcChartPlotArea.bottom + 1 );
    //if ( rcBounds.PtInRect(pt) )
    //{
    //	int x = UnscaleX( SnapScreenX(pt.x ));
    //	int y = UnscaleY( SnapY(pt.y ));
    //	TRACE("%d, %d\n", x, y );
    //}


    if ( m_rcChartPlotArea.contains( pt ) )
	emit mousePositionChanged( SnapX(UnscaleX(pt.x())), SnapX(UnscaleY(pt.y())) );
}


void ControlPointsWidget::updateValue( int n )
{
    int dwPos = (n - m_nDeviceMin) *  POINTS_MAX / (m_nDeviceMax-m_nDeviceMin);
    if ( m_bReverse )
	dwPos = POINTS_MAX - dwPos;
    if ( dwPos != m_nValue )
    {
	m_nValue = dwPos;
	update();
    }
}


QPoint ControlPointsWidget::ClipPoint( const QRect &rc, const QPoint &pt_in )
{
    QPoint pt = pt_in;

    if ( pt.x() < rc.left() )
	pt.setX(rc.left());
    else if ( pt.x() > rc.right() - 1 )
	pt.setX(rc.right()-1);

    if ( pt.y() < rc.top() )
	pt.setY( rc.top() );
    else if ( pt.y() > rc.bottom() - 1 )
	pt.setY( rc.bottom() - 1 );

    return pt;
}


void ControlPointsWidget::setSnap( bool bOn )
{
    m_bSnap = bOn;
}


void ControlPointsWidget::setDeviceMin( int n )
{
    m_nDeviceMin = n;
    update();
}

void ControlPointsWidget::setDeviceMax( int n )
{
    m_nDeviceMax = n;
    update();
}

void ControlPointsWidget::setReverse( bool b )
{
    m_bReverse = b;
    update();
}

void ControlPointsWidget::resizeEvent( QResizeEvent * )
{
    m_bInit = false;
    m_bHasWorkingData = false;
    update();
}

