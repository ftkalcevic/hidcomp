#ifndef _HID_H_
#define _HID_H_

#ifdef _WIN32
#pragma warning(push, 1)
#endif
#include <QDomElement>
#include <QList>
#ifdef _WIN32
#pragma warning(pop)
#pragma warning(disable:4251)
#endif

#include <vector>

#include "log.h"
#include "cpoints.h"
#include "lcddisplaydata.h"

namespace HIDItemType
{
    enum HIDItemType
    {
	Button		= 0,
	Value		= 1,
	Hatswitch	= 2,
	LED		= 3,
	LCD		= 4,
	OutputValue	= 5
    };
}


class HIDDeviceCriteria
{
public:
    HIDDeviceCriteria(void);
    ~HIDDeviceCriteria(void);

    bool bPID;
    unsigned short nPID;
    bool bVID;
    unsigned short nVID;
    bool bManufacturer;
    QString sManufacturer;
    bool bProduct;
    QString sProduct;
    bool bSerialNumber;
    QString sSerialNumber;
    bool bSystemId;
    QString sSystemId;

    void ReadXML( QDomElement pNode );
    QDomElement WriteXML( QDomElement pNode );
};

class HIDItem
{
public:
    HIDItem( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName, HIDItemType::HIDItemType type );
    HIDItem( const HIDItem &other );
    virtual ~HIDItem();

    int nIndex;
    unsigned short nUsagePage;
    unsigned short nUsage;
    bool bEnabled;
    QString sPinName;
    HIDItemType::HIDItemType type;

    virtual void ReadXML( QDomElement /*pNode*/ ) {}
    virtual QDomElement WriteXML( QDomElement pNode );
    static HIDItem *CreateFromXML( QDomElement pNode );
    static HIDItem *CreateItem( HIDItemType::HIDItemType type, int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
};

class HIDInputItem: public HIDItem
{
public:
    HIDInputItem( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName, HIDItemType::HIDItemType type );
    virtual ~HIDInputItem();
};

class HIDOutputItem: public HIDItem
{
public:
    HIDOutputItem( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName, HIDItemType::HIDItemType type );
    HIDOutputItem( const HIDOutputItem &other );
    virtual ~HIDOutputItem();
};

class HIDButton: public HIDInputItem
{
public:
    HIDButton( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
    virtual ~HIDButton();
};


namespace ValueType
{
    enum ValueType
    {
	Default
    };
};

namespace ValueOutputType
{
    enum ValueOutputType
    {
	FloatingPoint,
	RawInteger
    };
};

class HIDInputValue: public HIDInputItem
{
public:
    HIDInputValue( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
    virtual ~HIDInputValue();
    virtual void ReadXML( QDomElement pNode );
    virtual QDomElement WriteXML( QDomElement pNode );
    int Interpolate( int lP );
    int Scale( int lP );
    double DScale( int lP );

    ValueType::ValueType valueType;
    ValueOutputType::ValueOutputType outputType;
    int nLogicalMinOverride;
    int nLogicalMaxOverride;
    double dOutputMin;
    double dOutputMax;
    bool bUseResponseCurve;
    bool bReverse;
    std::vector<CPoint> responseCurve;
#define POINTS_MAX_VAL		10000		// 100% 
};

class HIDHatswitch: public HIDInputItem
{
public:
    HIDHatswitch( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
    virtual ~HIDHatswitch();
};

class HIDLED: public HIDOutputItem
{
public:
    HIDLED( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
    virtual ~HIDLED();
};

class HIDOutputValue: public HIDOutputItem
{
public:
    HIDOutputValue( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
    virtual ~HIDOutputValue();
};

class LCDEntry
{
public:
    LCDEntry( ELCDDisplayData::ELCDDisplayData eData, int nIndex, int nRow, int nCol, const QString &sFormat, double dScale, const QString &sTestData );
    LCDEntry( const LCDEntry &that );
    ~LCDEntry();
    QDomElement WriteXML( QDomElement pNode );
    void ReadXML( QDomElement pNode );

    ELCDDisplayData::ELCDDisplayData data() { return m_eData; }
    void setData( ELCDDisplayData::ELCDDisplayData e) { m_eData = e; }
    int index() { return m_nIndex; }
    void setIndex(int n) { m_nIndex = n; }
    int row() { return m_nRow; }
    void setRow(int n) { m_nRow = n; }
    int col() { return m_nCol; }
    void setCol(int n) { m_nCol = n; }
    QString format() { return m_sFormat; }
    void setFormat(QString s) { m_sFormat = s; }
    double scale() { return m_dScale; }
    void setScale(double d) { m_dScale = d; }
    QString testData() { return m_sTestData; }
    void setTestData(QString s) { m_sTestData = s; }

private:
    ELCDDisplayData::ELCDDisplayData m_eData;   // The data that is to be displayed
    int m_nIndex;           // The index of the data item, eg Position[Axis0]
    int m_nRow;
    int m_nCol;
    QString m_sFormat;      // Format String
    double m_dScale;
    QString m_sTestData;
};

class LCDPage
{   
public:
    LCDPage( int nNumber, const QString &sName );
    LCDPage( const LCDPage &that );
    ~LCDPage();
    QDomElement WriteXML( QDomElement pNode );
    void ReadXML( QDomElement pNode );

    int number() { return m_nNumber; }
    void setNumber( int n ) { m_nNumber = n; }
    QString name() { return m_sName; }
    void setName( const QString &s ) { m_sName = s; }
    QList<LCDEntry*> &data() { return m_data; }

private:
    int m_nNumber;
    QString m_sName;
    QList<LCDEntry *> m_data;
};

class HIDLCD: public HIDOutputItem
{
public:
    HIDLCD( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName );
    HIDLCD( const HIDLCD &other );
    HIDLCD & operator= ( const HIDLCD & other );
    virtual ~HIDLCD();

    virtual void ReadXML( QDomElement pNode );
    virtual QDomElement WriteXML( QDomElement pNode );

    int samplePeriod() { return m_nSamplePeriod; }
    void setSamplePeriod( int n ) { m_nSamplePeriod = n; }
    QList<LCDPage *> &pages() { return m_pages; }

private:
    int m_nSamplePeriod;        // ms
    QList<LCDPage *> m_pages;
    void clear();
};

class HID
{
public:
    HID();
    ~HID();

    HIDDeviceCriteria *criteria;
    std::vector< HIDItem *> items;
    QString configFile;

    static HID *CreateFromXML( const QString &file );
    QDomDocument MakeXML();
};

inline QString FormatPID( unsigned short n ) { return QString("%1").arg(n,4,16,QChar('0')); }
inline QString FormatVID( unsigned short n ) { return QString("%1").arg(n,4,16,QChar('0')); }

#endif
