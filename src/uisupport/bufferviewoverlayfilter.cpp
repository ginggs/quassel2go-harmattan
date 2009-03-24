/***************************************************************************
 *   Copyright (C) 2005-09 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "bufferviewoverlayfilter.h"

#include "bufferviewoverlay.h"
#include "networkmodel.h"
#include "types.h"

BufferViewOverlayFilter::BufferViewOverlayFilter(QAbstractItemModel *model, BufferViewOverlay *overlay)
  : QSortFilterProxyModel(model),
    _overlay(0)
{
  setOverlay(overlay);
  setSourceModel(model);

  setDynamicSortFilter(true);
}

void BufferViewOverlayFilter::setOverlay(BufferViewOverlay *overlay) {
  if(_overlay == overlay)
    return;

  if(_overlay) {
    disconnect(_overlay, 0, this, 0);
  }

  _overlay = overlay;

  if(!overlay) {
    invalidate();
    return;
  }

  connect(overlay, SIGNAL(destroyed()), this, SLOT(overlayDestroyed()));
  connect(overlay, SIGNAL(hasChanged()), this, SLOT(invalidate()));
  invalidate();
}

void BufferViewOverlayFilter::overlayDestroyed() {
  setOverlay(0);
}

bool BufferViewOverlayFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
  if(!_overlay)
    return false;

  QModelIndex source_bufferIndex = sourceModel()->index(source_row, 0, source_parent);

  if(!source_bufferIndex.isValid()) {
    qWarning() << "filterAcceptsRow has been called with an invalid Child";
    return false;
  }

  NetworkId networkId =  sourceModel()->data(source_bufferIndex, NetworkModel::NetworkIdRole).value<NetworkId>();
  if(!_overlay->networkIds().contains(networkId) && ! _overlay->allNetworks())
    return false;

  int activityLevel = sourceModel()->data(source_bufferIndex, NetworkModel::BufferActivityRole).toInt();
  if(_overlay->minimumActivity() > activityLevel)
    return false;

  bool isActive = sourceModel()->data(source_bufferIndex, NetworkModel::ItemActiveRole).toBool();
  if(_overlay->hideInactiveBuffers() && !isActive && activityLevel <= BufferInfo::OtherActivity)
    return false;

  int bufferType = sourceModel()->data(source_bufferIndex, NetworkModel::BufferTypeRole).toInt();
  if(!(_overlay->allowedBufferTypes() & bufferType))
    return false;

  BufferId bufferId = sourceModel()->data(source_bufferIndex, NetworkModel::BufferIdRole).value<BufferId>();
  Q_ASSERT(bufferId.isValid());

  if(_overlay->bufferIds().contains(bufferId))
    return true;

  if(_overlay->tempRemovedBufferIds().contains(bufferId))
    return activityLevel > BufferInfo::OtherActivity;

  if(_overlay->removedBufferIds().contains(bufferId))
    return false;

  // the buffer is not known to us
  return _overlay->addBuffersAutomatically();
}

