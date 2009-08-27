#ifndef EDITFONTDLG_H
#define EDITFONTDLG_H

#include <QDialog>
#include "ui_editfontdlg.h"
#include "hid.h"

class EditFontDlg : public QDialog
{
    Q_OBJECT

public:
    EditFontDlg( const QList<LCDFont *> &fonts, int nMinFontIndex, int nMaxFontIndex, QWidget *parent = 0);
    ~EditFontDlg();

public slots:
    void onClearFont();
    void onUserFontCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    QList<LCDFont *> fontData();

private:
    Ui::EditFontDlgClass ui;

    QList<LCDFont *> m_fonts;
    int m_nMinFontIndex;
    int m_nMaxFontIndex;
    int m_nRows;
    int m_nCols;
};

#endif // EDITFONTDLG_H
