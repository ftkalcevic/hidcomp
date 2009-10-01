#ifndef _LCDPROC_H_
#define _LCDPROC_H_

namespace ELCDCmd
{
    enum ELCDCmd
    {   
	ClearScreen,
	SetBackground,
	SetForeground,
	Text,
	SetFont,
	SetBacklight,
	Rectangle,
    };
};

struct LCDCmd
{
    LCDCmd( ELCDCmd::ELCDCmd cmd ) : nCmd(cmd) {}
    ELCDCmd::ELCDCmd nCmd;
};

struct LCDClearScreen: LCDCmd
{
    LCDClearScreen() : LCDCmd( ELCDCmd::ClearScreen ) {}
};

struct LCDSetBackground: LCDCmd
{
    LCDSetBackground( int _r, int _g, int _b ) : LCDCmd(ELCDCmd::SetBackground), r(_r), g(_g), b(_b) {}
    int r, g, b;
};

struct LCDSetForeground: LCDCmd
{
    LCDSetForeground( int _r, int _g, int _b ) : LCDCmd(ELCDCmd::SetForeground), r(_r), g(_g), b(_b) {}
    int r, g, b;
};

struct LCDText: LCDCmd
{
    LCDText( int _x, int _y, QString &_s, bool _pixel ) : LCDCmd(ELCDCmd::Text), x(_x), y(_y), s(_s), pixel_coord(_pixel) {}
    int x, y;
    QString s;
    bool pixel_coord;
};

struct LCDSetFont: LCDCmd
{
    LCDSetFont( int _f ) : LCDCmd(ELCDCmd::SetFont), font(_f) {}
    int font;
};

struct LCDSetBacklight: LCDCmd
{
    LCDSetBacklight( int _i ) : LCDCmd(ELCDCmd::SetBacklight), intensity(_i) {}
    int intensity;
};

struct LCDRectangle: LCDCmd
{
    LCDRectangle( int _x, int _y, int _w, int _h, bool _f ) : LCDCmd(ELCDCmd::Rectangle), x(_x), y(_y), width(_w), height(_h), fill(_f) {}
    int x, y;
    int width, height;
    bool fill;
};

class LCDProc: public QThread
{
public:
    LCDProc(int nPort);
    ~LCDProc(void);

    bool Initialise();
    LCDCmd *getQueueMessage();
    int queueLength() const ;

private:
    virtual void run();
    const QString ProcessCommand( QString sLine );
    void queueMessage( LCDCmd *pCmd );

    int m_nPort;
    Logger m_Logger;
    QTcpServer *m_tcpServer;
    bool m_bRun;

    QMutex m_BufferMutex;
    QList<LCDCmd *> m_Buffer;
};

#endif
