#ifndef __XMLUTILITY_H__
#define __XMLUTILITY_H__

#include <QDomElement>

class XMLUtility
{
public:
    XMLUtility(void);
    ~XMLUtility(void);

    static void setAttribute( QDomElement &pNode, const char *sName, const QString sValue );
    static void setAttribute( QDomElement &pNode, const char *sName, int nValue );
    static void setAttribute( QDomElement &pNode, const char *sName, double nValue );
    static void setAttribute( QDomElement &pNode, const char *sName, bool bValue );

    static bool getAttributeString( QDomElement &pNode, const char *sName, QString &s );
    static QString getAttribute( QDomElement &pNode, const char *sName, const char *sDefault );
    static int getAttribute( QDomElement &pNode, const char *sName, int nDefault );
    static double getAttribute( QDomElement &pNode, const char *sName, double nDefault );
    static bool getAttribute( QDomElement &pNode, const char *sName, bool bDefault );

    static QDomElement firstChildElement(QDomElement &pNode, const char *sTag);
    static QDomNodeList elementsByTagName(QDomElement &pNode, const char *sTag);
    static bool hasAttribute(QDomElement &pNode, const char *sTag);
};


#endif
