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

#ifndef CONTROLPOINTSWIDGET_H
#define CONTROLPOINTSWIDGET_H

#include "cpoints.h"

class ControlPointsWidget : public QWidget
{
	Q_OBJECT

public:
	ControlPointsWidget(QWidget *parent);
	~ControlPointsWidget();
	void setPoints( std::vector<CPoint> &points );
	std::vector<CPoint> points() const { return m_Points; }
	void setSnap( bool bOn );
	bool changed() const { return m_bChanged; }
	void setDeviceMin( int n );
	void setDeviceMax( int n );
	void setReverse( bool b );

signals:
	void mousePositionChanged( int x, int y );

public slots:
	void updateValue( int n );

private:
	virtual void paintEvent( QPaintEvent * evnt );
	virtual void mousePressEvent ( QMouseEvent * evnt );
	virtual void mouseMoveEvent ( QMouseEvent * evnt );
	virtual void mouseReleaseEvent( QMouseEvent * evnt ) ;
	virtual void focusInEvent(QFocusEvent * evnt);
	virtual void focusOutEvent(QFocusEvent * evnt);
	virtual void resizeEvent ( QResizeEvent * evnt );

	void SetMouseCursor( const QPoint &pos );
	void CalculateWorkingData();
	int ScaleX( int x );
	int ScaleY( int y );
	int UnscaleX( int x );
	int UnscaleY( int y );
	int Interpolate( int lP );
	int SnapX( int lP );
	int SnapY( int logY );
	static QPoint ClipPoint( const QRect &rc, const QPoint &pt_in );

	bool m_bChanged;
	bool m_bInit;
	bool m_bHasFocus;
	bool m_bHasWorkingData;
	bool m_bSnap;
	bool m_bDragging;
	int m_nValue;
	int m_nFocusPoint;		// the point that is currently under the cursor.
	int m_nDeviceMin;
	int m_nDeviceMax;
	bool m_bReverse;
	QRect m_rcChart;
	QRect m_rcChartPlotArea;
	QRect m_rcChartHorzCentre;
	QRect m_rcChartVertCentre;
	QRect m_rcTestBar;
	QRect m_rcTestBarText;
	QColor m_IndicatorColour;
	double m_dScaleX;
	double m_dScaleY;
	std::vector<QRect> m_rcScreenPoints;
	std::vector<CPoint> m_Points;
	QPoint m_ptDragCPoint;		// Where we start draging from
	QMenu *m_pPopupMenu;
	QAction *m_pAdd;
	QAction *m_pRemove;
};

#endif // CONTROLPOINTSWIDGET_H
