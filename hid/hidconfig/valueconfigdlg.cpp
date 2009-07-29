#include "valueconfigdlg.h"


const int OUTPUT_FloatingPoint = 0;
const int OUTPUT_RawInteger = 1;

const int POINTS_CUSTOM		= 0;
const int POINTS_DEFAULT_1	= 1;
const int POINTS_DEFAULT_2	= 2;
const int POINTS_DEFAULT_3	= 3;
const int POINTS_DEFAULT_4	= 4;

ValueConfigDlg::ValueConfigDlg(HID_ReportItem_t *item, QWidget *parent)
	: QDialog(parent),
	  m_pHIDItem( item )
{
	ui.setupUi(this);
	
	CreateUI();

	// Set defaults
	m_nWhichPoints = POINTS_DEFAULT_1;
	ui.cpointView->setPoints( m_Points[m_nWhichPoints] );

	ui.txtLogicalMinOverride->setText( tr("%1").arg( item->Attributes.LogicalMinimum ) );
	ui.txtLogicalMaxOverride->setText( tr("%1").arg( item->Attributes.LogicalMaximum) );

	ui.cboOutputType->setCurrentIndex( ValueOutputType::FloatingPoint );

	ui.txtMaxOutput->setText( "1.0" );
	ui.txtMinOutput->setText( "-1.0" );

	ui.cpointView->setDeviceMin( item->Attributes.LogicalMinimum );
	ui.cpointView->setDeviceMax( item->Attributes.LogicalMaximum );
	
	ui.chkUseCPoints->setChecked( false );
	ui.cpointView->setEnabled( false );
}


ValueConfigDlg::ValueConfigDlg( HID_ReportItem_t *item, QWidget *parent, bool bUseResponseCurve, std::vector<CPoint> &responseCurve, int nLogicalMinOverride, int nLogicalMaxOverride,
							   ValueType::ValueType valueType, ValueOutputType::ValueOutputType valueOutputType, double dOutputMin, double dOutputMax, bool bReverseAxis )
	: QDialog(parent),
	  m_pHIDItem( item )
{
	ui.setupUi(this);
	
	CreateUI();

	m_nWhichPoints = POINTS_CUSTOM;
	m_Points[POINTS_CUSTOM] = responseCurve;
	ui.cpointView->setPoints( m_Points[m_nWhichPoints] );
	ui.cpointView->setDeviceMin( item->Attributes.LogicalMinimum );
	ui.cpointView->setDeviceMax( item->Attributes.LogicalMaximum );
	ui.cpointView->setReverse( bReverseAxis );

	ui.chkUseCPoints->setChecked( bUseResponseCurve );
	ui.cpointView->setEnabled( bUseResponseCurve );

	ui.txtLogicalMinOverride->setText( tr("%1").arg(nLogicalMinOverride) );
	ui.txtLogicalMaxOverride->setText( tr("%1").arg(nLogicalMaxOverride) );

	selectComboItem( ui.cboType, valueType );
	selectComboItem( ui.cboOutputType, valueOutputType );

	ui.txtMaxOutput->setText( tr("%1").arg( dOutputMax ) );
	ui.txtMinOutput->setText( tr("%1").arg( dOutputMin ) );
}


// Do manual creation extras here.
void ValueConfigDlg::CreateUI()
{
	ui.txtLogicalMaxOverride->setValidator( new QIntValidator(ui.txtLogicalMaxOverride) );
	ui.txtLogicalMinOverride->setValidator( new QIntValidator(ui.txtLogicalMinOverride) );
	ui.txtMinOutput->setValidator( new QDoubleValidator(ui.txtMinOutput) );
	ui.txtMaxOutput->setValidator( new QDoubleValidator(ui.txtMaxOutput) );

	ui.cboType->addItem( "Default", QVariant(ValueType::Default) );

	ui.cboOutputType->addItem( "Floating Point", QVariant(ValueOutputType::FloatingPoint) );
	ui.cboOutputType->addItem( "Raw Integer", QVariant(ValueOutputType::RawInteger) );

	ui.lblLogicalMaxDefault->setText( tr("%1").arg( m_pHIDItem->Attributes.LogicalMaximum ) );
	ui.lblLogicalMinDefault->setText( tr("%1").arg( m_pHIDItem->Attributes.LogicalMinimum ) );

	std::vector<CPoint> m_Points1, m_Points2, m_Points3, m_Points4;
	m_Points1.push_back( CPoint( 0, 0 ) );
	m_Points1.push_back( CPoint( 1000, 0 ) );
	m_Points1.push_back( CPoint( 2500, 2500 ) );
	m_Points1.push_back( CPoint( 4000, 5000 ) );
	m_Points1.push_back( CPoint( 6000, 5000 ) );
	m_Points1.push_back( CPoint( 7500, 7500 ) );
	m_Points1.push_back( CPoint( 9000, 10000 ) );
	m_Points1.push_back( CPoint( 10000, 10000 ) );

	m_Points2.push_back( CPoint( 0, 0 ) );
	m_Points2.push_back( CPoint( 500, 2500 ) );
	m_Points2.push_back( CPoint( 2000, 4500 ) );
	m_Points2.push_back( CPoint( 4000, 5000 ) );
	m_Points2.push_back( CPoint( 6000, 5000 ) );
	m_Points2.push_back( CPoint( 8000, 5500 ) );
	m_Points2.push_back( CPoint( 9500, 7500 ) );
	m_Points2.push_back( CPoint( 10000, 10000 ) );

	m_Points3.push_back( CPoint( 0, 0 ) );
	m_Points3.push_back( CPoint( 1428, 1428 ) );
	m_Points3.push_back( CPoint( 2857, 2857 ) );
	m_Points3.push_back( CPoint( 4285, 4285 ) );
	m_Points3.push_back( CPoint( 5714, 5714 ) );
	m_Points3.push_back( CPoint( 7142, 7142 ) );
	m_Points3.push_back( CPoint( 8571, 8571 ) );
	m_Points3.push_back( CPoint( 10000, 10000 ) );

	m_Points4.push_back( CPoint( 0, 0 ) );
	m_Points4.push_back( CPoint( 1554, 217 ) );
	m_Points4.push_back( CPoint( 3000, 500 ) );
	m_Points4.push_back( CPoint( 4500, 1000 ) );
	m_Points4.push_back( CPoint( 6750, 2000 ) );
	m_Points4.push_back( CPoint( 8500, 4000 ) );
	m_Points4.push_back( CPoint( 9500, 7500 ) );
	m_Points4.push_back( CPoint( 10000, 10000 ) );

	m_Points.push_back( m_Points1 );
	m_Points.push_back( m_Points1 );
	m_Points.push_back( m_Points2 );
	m_Points.push_back( m_Points3 );
	m_Points.push_back( m_Points4 );

	connect( ui.cpointView, SIGNAL(mousePositionChanged( int, int)), this, SLOT(onCPointPositionChanged( int, int)) );
}

ValueConfigDlg::~ValueConfigDlg()
{

}


void ValueConfigDlg::UpdateValue()
{
	ui.cpointView->updateValue( m_pHIDItem->Value );
}

void ValueConfigDlg::onOutputTypeChanged(int nIndex)
{
	bool bEnable;
	if ( nIndex == OUTPUT_FloatingPoint )
		bEnable = true;
	else
		bEnable = false;
	ui.txtMinOutput->setEnabled( bEnable );
	ui.txtMaxOutput->setEnabled( bEnable );
}


void ValueConfigDlg::onCPointPositionChanged( int x, int y)
{
	ui.lblCPointPos->setText( tr("(%1,%2)").arg((double)x/100.0).arg((double)y/100.0) );
}


void ValueConfigDlg::onSnapToGridClicked(bool bON)
{
	ui.cpointView->setSnap( bON );
}

void ValueConfigDlg::UpdatePoints( int nNewPoints )
{
	if ( ui.cpointView->changed() )
	{
		std::vector<CPoint> points = ui.cpointView->points();
		m_Points[POINTS_CUSTOM].assign( points.begin(), points.end());
	}
	m_nWhichPoints = nNewPoints;
	ui.cpointView->setPoints( m_Points[m_nWhichPoints] );
}

void ValueConfigDlg::onCPoint1()
{
	UpdatePoints( POINTS_DEFAULT_1 );
}

void ValueConfigDlg::onCPoint2()
{
	UpdatePoints( POINTS_DEFAULT_2 );
}

void ValueConfigDlg::onCPoint3()
{
	UpdatePoints( POINTS_DEFAULT_3 );
}

void ValueConfigDlg::onCPoint4()
{
	UpdatePoints( POINTS_DEFAULT_4 );
}

void ValueConfigDlg::onCPointCustom()
{
	UpdatePoints( POINTS_CUSTOM );
}

void ValueConfigDlg::onLogicalMinEditFinished()
{
	ui.cpointView->setDeviceMin( ui.txtLogicalMinOverride->text().toInt() );
}

void ValueConfigDlg::onLogicalMaxEditFinished()
{
	ui.cpointView->setDeviceMax( ui.txtLogicalMaxOverride->text().toInt() );
}

void ValueConfigDlg::onUseCPointsClicked(bool bUse)
{
	ui.cpointView->setEnabled( bUse );
}


void ValueConfigDlg::onReverseAxisClicked(bool bReverse)
{
	ui.cpointView->setReverse( bReverse );
}

bool ValueConfigDlg::useResponseCurve()
{
	return ui.chkUseCPoints->checkState() == Qt::Checked;
}

std::vector<CPoint> ValueConfigDlg::responseCurve()
{
	return ui.cpointView->points();
}

int ValueConfigDlg::logicalMinOverride()
{
	return ui.txtLogicalMinOverride->text().toInt();
}

int ValueConfigDlg::logicalMaxOverride()
{
	return ui.txtLogicalMaxOverride->text().toInt();
}

ValueType::ValueType ValueConfigDlg::type()
{
	return (ValueType::ValueType)ui.cboType->itemData( ui.cboType->currentIndex() ).toInt();
}

ValueOutputType::ValueOutputType ValueConfigDlg::outputType()
{
	return (ValueOutputType::ValueOutputType)ui.cboType->itemData( ui.cboType->currentIndex() ).toInt();
}

double ValueConfigDlg::outputMin()
{
	return ui.txtMinOutput->text().toDouble();
}

double ValueConfigDlg::outputMax()
{
	return ui.txtMaxOutput->text().toDouble();
}

bool ValueConfigDlg::reverse()
{
	return ui.chkReverse->checkState() == Qt::Checked;
}


void ValueConfigDlg::selectComboItem( QComboBox *pCombo, int nData )
{
	for ( int i = 0; i < pCombo->count(); i++ )
		if ( pCombo->itemData(i).toInt() == nData )
		{
			pCombo->setCurrentIndex( i );
			break;
		}
}

