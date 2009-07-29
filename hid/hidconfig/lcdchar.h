#ifndef _LCDCHAR_H_
#define _LCDCHAR_H_

#include <QPixmap>

class LCDChar
{
    QImage m_bmp, m_bmpHighlight;
    QRect m_rcSource;

public:
    enum { 
        PIXELS_X = 5,
        PIXELS_Y = 7,
        PIXEL_SIZE = 10
    };

    LCDChar( unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4, unsigned char b5 )
    {
        m_bmp = QImage( PIXEL_SIZE * PIXELS_X, PIXEL_SIZE * PIXELS_Y, QImage::Format_ARGB32 );
        m_bmp.fill( QColor(0,0,0,0).rgba() );
        m_bmpHighlight = QImage( PIXEL_SIZE * PIXELS_X, PIXEL_SIZE * PIXELS_Y, QImage::Format_ARGB32 );
        m_bmpHighlight.fill( QColor(0,0,0,0).rgba() );
        SetBits(0, m_bmp, m_bmpHighlight, b1);
        SetBits(1, m_bmp, m_bmpHighlight, b2);
        SetBits(2, m_bmp, m_bmpHighlight, b3);
        SetBits(3, m_bmp, m_bmpHighlight, b4);
        SetBits(4, m_bmp, m_bmpHighlight, b5);
    }

    void SetBits(int x, QImage &bmp, QImage &bmpHighlight, unsigned char data)
    {
        for (int i = 0; i < PIXELS_Y; i++)
        {
            if ( (data & (1 << i)) != 0 )
            {
                for (int a = 0; a < PIXEL_SIZE-2; a++)
                    for (int b = 0; b < PIXEL_SIZE - 2; b++)
                    {
                        bmp.setPixel(x * PIXEL_SIZE + a, i * PIXEL_SIZE + b, QColor(Qt::black).rgba() );
                        bmpHighlight.setPixel(x * PIXEL_SIZE + a, i * PIXEL_SIZE + b, QColor(Qt::darkMagenta).rgba() );
                    }
            }
        }
    }

    void Draw(QPainter &p, QRect rc, bool bHighlight)
    {
        p.setRenderHint( QPainter::HighQualityAntialiasing, true );
        p.setRenderHint( QPainter::SmoothPixmapTransform, true );
        if ( bHighlight )
            p.drawImage( rc, m_bmpHighlight );
        else
            p.drawImage( rc, m_bmp );
    }
};

#endif
