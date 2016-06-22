// hidcomp/hidconfig, HID device interface for emc
// Copyright (C) 2008, Frank Tkalcevic, www.franksworkshop.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"

#include "comboboxdelegate.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
        : QItemDelegate(parent)
{

}

ComboBoxDelegate::~ComboBoxDelegate()
{

}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable( false );
    editor->setFocusPolicy( Qt::StrongFocus );

    for ( QMap<int,QString>::const_iterator it = m_items.begin(); it != m_items.end(); it++ )
        editor->addItem( it.value(), it.key() );

    return editor;
}


void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //int value = index.model()->data(index, Qt::EditRole).toInt();
    //QComboBox *combo = static_cast<QComboBox*>(editor);
    //combo->setCurrentIndex(value);

    int nId = index.model()->data(index, Qt::UserRole).toInt();
    QComboBox *combo = static_cast<QComboBox*>(editor);
    int nIndex = combo->findData( QVariant(nId), Qt::UserRole );
    combo->setCurrentIndex(nIndex);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //QComboBox *combo = static_cast<QComboBox*>(editor);
    //int value = combo->currentIndex();
    //model->setData(index, value, Qt::EditRole);

    QComboBox *combo = static_cast<QComboBox*>(editor);
    int nIndex = combo->currentIndex();
    int nId = combo->itemData( nIndex, Qt::UserRole ).toInt();
    QString sValue = combo->itemText( nIndex );
    model->setData(index, sValue, Qt::EditRole);
    model->setData(index, nId, Qt::UserRole);
    model->setData(index, "Tool tip", Qt::ToolTip );
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
