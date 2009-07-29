#include "common.h"
#include "usages.h"
#include "log.h"

#ifdef _WIN32
#pragma warning(push, 1)
#endif

#include <QDomDocument>
#include <QFile>
#include <QResource>

#ifdef _WIN32
#pragma warning(pop)
#pragma warning(disable:4251)
#endif

#include "xmlutility.h"

#ifdef _WIN32
// disable warning about _snprintf
#pragma warning(disable:4996)
#endif

#define USAGES_DATA		":/usbhidlib/Usages"

bool Usages::m_bInitialised = false;
std::vector<UsagePage*> Usages::m_UsagePages;
std::map<unsigned short, UsagePage*> Usages::m_UsagePageMap;
Logger *Usages::m_Logger = NULL;


Usage::Usage(QString sName, unsigned short nId)
{
    m_sName = sName;
    m_nId = nId;
}


UsagePage::UsagePage(QString sName, unsigned short nId)
{
    m_sName = sName;
    m_nId = nId;
}

void UsagePage::AddUsage( Usage *u )
{
    m_Usages.push_back(u);
    m_UsageMap[u->m_nId] = u;
}

QString UsagePage::GetUsageString(unsigned short nUsage)
{
    std::map<unsigned short, Usage*>::iterator it = m_UsageMap.find(nUsage);
    if ( it != m_UsageMap.end() )
	return it->second->m_sName;
    else
    {
	return QString().setNum( nUsage );
    }
}


Usages::Usages(void)
{
}

Usages::~Usages(void)
{
}

void Usages::GetUsages( unsigned short nPage, unsigned short nUsage, QString &sPage, QString &sUsage )
{
    if ( !m_bInitialised )
	LoadUsages();

    std::map<unsigned short, UsagePage*>::iterator it = m_UsagePageMap.find(nPage);
    if ( it != m_UsagePageMap.end() )
    {
	UsagePage *page = it->second;
	sPage = page->m_sName;
	sUsage = page->GetUsageString(nUsage);
    }
    else
    {
	sPage.setNum( nPage );
	sUsage.setNum( nUsage );
    }
}


void Usages::LoadUsages()
{
    if ( m_bInitialised )
        return;
    Q_INIT_RESOURCE(usbhid);

    // Mark initialised even though it may fail to initialise.  No point repeating the same error.
    m_bInitialised = true;

    m_Logger = new Logger( "Usages", "Usages" );

    QFile file( USAGES_DATA );
    file.open(QIODevice::ReadOnly);

    QDomDocument doc("Usages");
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomElement rootElement = doc.firstChildElement( "Usages" );
    if ( rootElement.isNull() )
    {
        LOG_MSG( *m_Logger, LogTypes::Error, "Can't find root node 'Usages' in usage.xml" );
        return;
    }

    QDomNodeList pages = rootElement.elementsByTagName( "UsagePage" );
    for ( uint i = 0; i < pages.length(); i++ )
    {
        QDomElement page = pages.item(i).toElement();

        if ( !page.isNull() )
        {
            QString sUsagePageName = XMLUtility::getAttribute( page, "Name", "" );
            unsigned short nUsagePageId = XMLUtility::getAttribute( page, "Id", 0 );

            if ( sUsagePageName.isEmpty() || nUsagePageId <= 0 )
            {
                LOG_MSG( *m_Logger, LogTypes::Error, "Error reading usage.xml.  Missing UsagePage, or invalid id." );
            }
            else
            {
                UsagePage *pPage = new UsagePage( sUsagePageName, nUsagePageId );
                m_UsagePages.push_back( pPage );
                m_UsagePageMap[nUsagePageId] = pPage;

                QDomNodeList usages = page.elementsByTagName( "Usage" );
                for ( uint j = 0; j < usages.length(); j++ )
                {
                    QDomElement usage = usages.item(j).toElement();

                    if ( !usage.isNull() )
                    {
                        QString sUsageName = XMLUtility::getAttribute( usage, "Name", "" );
                        unsigned short nUsageId = XMLUtility::getAttribute( usage, "Id", 0 );

                        if ( sUsageName.isEmpty() || nUsageId <= 0 )
                        {
                            LOG_MSG( *m_Logger, LogTypes::Error, "Error reading usage.xml.  Missing Usage, or invalid id." );
                        }
                        else
                        {
                            Usage *pUsage = new Usage(sUsageName, nUsageId );
                            pPage->AddUsage( pUsage );
                        }
                    }
                }
            }
        }
    }
}
