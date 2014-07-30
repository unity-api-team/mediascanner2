/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    James Henstridge <james.henstridge@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SongsSearchModel.hh"
#include <exception>
#include <QDebug>

using namespace mediascanner::qml;

SongsSearchModel::SongsSearchModel(QObject *parent)
    : MediaFileModelBase(parent), query("") {
}

QString SongsSearchModel::getQuery() {
    return query;
}

void SongsSearchModel::setQuery(const QString query) {
    if (this->query != query) {
        this->query = query;
        invalidate();
    }
}

std::unique_ptr<StreamingModel::RowData> SongsSearchModel::retrieveRows(std::shared_ptr<MediaStoreBase> store, int /*limit*/, int offset) const {
    std::vector<mediascanner::MediaFile> songs;
    // No batching support, so only send results for the zero offset.
    if (offset == 0) {
        try {
            songs = store->query(query.toStdString(), mediascanner::AudioMedia);
        } catch (const std::exception &e) {
            qWarning() << "Failed to retrieve song search results:" << e.what();
        }
    }
    return std::unique_ptr<StreamingModel::RowData>(
        new MediaFileRowData(std::move(songs)));
}
