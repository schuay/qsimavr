/* *************************************************************************
 *  Copyright 2012 Jakob Gruber                                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */



#include "plugintablemodel.h"

#include <assert.h>

PluginTableModel::PluginTableModel(PluginManager *manager) : manager(manager)
{
}

int PluginTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return manager->count();
}

int PluginTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return COL_COUNT;
}

QVariant PluginTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::CheckStateRole:
        switch (index.column()) {
        case COL_ENABLED: return manager->enabled(index.row()) ? Qt::Checked : Qt::Unchecked;
        case COL_VCD: return manager->vcd(index.row()) ? Qt::Checked : Qt::Unchecked;
        default: break;
        }
        break;
    case Qt::DisplayRole:
        switch (index.column()) {
        case COL_NAME: return manager->name(index.row());
        default: break;
        }
        break;
    }

    return QVariant();
}

QVariant PluginTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case COL_NAME: return QVariant("Name");
        case COL_ENABLED: return QVariant("Enabled");
        case COL_VCD: return QVariant("VCD");
        default: assert(0);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool PluginTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case Qt::CheckStateRole:
        switch (index.column()) {
        case COL_ENABLED: manager->setEnabled(index.row(), static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked); break;
        case COL_VCD: manager->setVcd(index.row(), static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked); break;
        default: return false;
        }
        break;
    default:
        return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags PluginTableModel::flags(const QModelIndex &index) const
{
    if (index.column() != COL_ENABLED && index.column() != COL_VCD) {
        return QAbstractItemModel::flags(index);
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}
