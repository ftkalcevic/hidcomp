#ifndef _HIDUIVALUE_H_
#define _HIDUIVALUE_H_

#include "hiddevices.h"
#include "hiduibase.h"
#include "valueconfigdlg.h"

class HIDUIValue: public QWidget, public HIDUIBase
{
    Q_OBJECT

public:
    HIDUIValue(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent=NULL);
    ~HIDUIValue(void);

    virtual void Update();
    virtual void setConfig( HIDItem *pItem );
    virtual void getConfig( HIDItem *pItem );
    virtual QList<QString> PinNames( const QString &sPrefix );

public slots:
    void onConfigClicked(bool);

signals:
    void configValue( HIDUIValue *);

private:
    QProgressBar *m_pValue;

    ValueConfigDlg *m_pValueConfigDlg;
    bool m_bUseResponseCurve;
    std::vector<CPoint> m_responseCurve;
    int m_nLogicalMinOverride;
    int m_nLogicalMaxOverride;
    ValueType::ValueType m_ValueType;
    ValueOutputType::ValueOutputType m_ValueOutputType;
    double m_dOutputMin;
    double m_dOutputMax;
    bool m_bReverseAxis;

    void CreateUI( HIDDevice *pDev, QGridLayout *layout, int nRow );
};


#endif
