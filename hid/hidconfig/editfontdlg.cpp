#include "stdafx.h"
#include "editfontdlg.h"

EditFontDlg::EditFontDlg( const QList<LCDFont *> &fonts, int nMinFontIndex, int nMaxFontIndex, QWidget *parent)
: QDialog(parent)
, m_nMinFontIndex(nMinFontIndex)
, m_nMaxFontIndex(nMaxFontIndex)
, m_nRows(7)
, m_nCols(5)
{
    ui.setupUi(this);

    // fonts come in as a list of LCDFont objects, which include the font id and data,
    // only for fonts that exist.  We expand this into a the full list we are working with
    for ( int index = nMinFontIndex; index <= nMaxFontIndex; index++ )
    {
	bool bFound = false;
	for ( int i = 0; i < fonts.count(); i++ )
	    if ( fonts[i]->index() == index )
	    {
		m_fonts.push_back( new LCDFont( *(fonts[i]) ) );
		bFound = true;
		break;
	    }
	if ( !bFound )
	    m_fonts.push_back( new LCDFont( index, QVector<byte>( m_nCols ) ) );
    }
    ui.lcdFont->setSize( m_nRows, m_nCols );

    // populate the table.  Add one entry for every possible user font (expect this to be 8)
    ui.tableChars->setColumnWidth(0, 100);
    for ( int i = nMinFontIndex; i <= nMaxFontIndex; i++ )
    {
        int nRow = ui.tableChars->rowCount();
        ui.tableChars->insertRow( nRow );
        ui.tableChars->setItem(nRow, 0, new QTableWidgetItem(QString::number(i), QMetaType::UInt) );
    }
    if ( ui.tableChars->rowCount() > 0 )
	ui.tableChars->selectRow(0);
}

EditFontDlg::~EditFontDlg()
{

}


void EditFontDlg::onClearFont()
{
    ui.lcdFont->clearFont();
}


void EditFontDlg::onUserFontCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if ( previous != NULL )
    {
	int nFontChar = previous->text().toInt();
	m_fonts[nFontChar]->setData( ui.lcdFont->data() );
    }

    int nFontChar = current->text().toInt();
    ui.lcdFont->setData( m_fonts[nFontChar]->data() );
}


QList<LCDFont *> EditFontDlg::fontData()
{
    QTableWidgetItem *item = ui.tableChars->currentItem();
    if ( item != NULL )
    {
	int nFontChar = item->text().toInt();
	m_fonts[nFontChar]->setData( ui.lcdFont->data() );
    }

    QList<LCDFont *> ret;
    foreach ( LCDFont *pFont, m_fonts )
    {
	bool bEmpty = true;
	for ( int i = 0; i < pFont->data().count() && bEmpty; i++ )
	    bEmpty = (pFont->data()[i] == 0);
	if ( !bEmpty )
	    ret.push_back( new LCDFont( *pFont ) );
    }
    return ret;
}

