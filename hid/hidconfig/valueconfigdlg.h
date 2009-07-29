#ifndef VALUECONFIGDLG_H
#define VALUECONFIGDLG_H

#include "ui_valueconfigdlg.h"
#include "hid.h"
#include "hidparser.h"

class ValueConfigDlg : public QDialog
{
    Q_OBJECT

public:
    ValueConfigDlg(HID_ReportItem_t *item, QWidget *parent = 0);
    ValueConfigDlg(HID_ReportItem_t *item, QWidget *parent, bool bUseResponseCurve, std::vector<CPoint> &responseCurve, int nLogicalMinOverride, int nLogicalMaxOverride,
        ValueType::ValueType valueType, ValueOutputType::ValueOutputType valueOutputType, double dOutputMin, double dOutputMax, bool bReverseAxis );
    ~ValueConfigDlg();

public:
    void UpdateValue();

    bool useResponseCurve();
    std::vector<CPoint> responseCurve();
    int logicalMinOverride();
    int logicalMaxOverride();
    ValueType::ValueType type();
    ValueOutputType::ValueOutputType outputType();
    double outputMin();
    double outputMax();
    bool reverse();

public slots:
    void onOutputTypeChanged(int);
    void onCPointPositionChanged( int x, int y);
    void onSnapToGridClicked(bool);
    void onCPoint1();
    void onCPoint2();
    void onCPoint3();
    void onCPoint4();
    void onCPointCustom();
    void onLogicalMinEditFinished();
    void onLogicalMaxEditFinished();
    void onUseCPointsClicked(bool);
    void onReverseAxisClicked(bool);

private:
    Ui::ValueConfigDlg ui;
    HID_ReportItem_t *m_pHIDItem;
    std::vector<std::vector<CPoint> > m_Points;
    int m_nWhichPoints;

    void CreateUI();
    void UpdatePoints( int nNewPoints );
    void selectComboItem( QComboBox *pCombo, int nData );
};

#endif // VALUECONFIGDLG_H
