/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
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

#include"qtclient.h"
#include"qtservice.h"
#include<mediascanner/Album.hh>
#include<QDBusReply>
#include<QDebug>

using namespace mediascanner;

QtClient::QtClient(QObject *parent) : QObject(parent),
    service(MS_DBUS_NAME, MS_DBUS_OBJECT, MS_DBUS_INTERFACE) {

}

QtClient:: ~QtClient() {
}


mediascanner::MediaFile QtClient::lookup(const std::string &filename) const {
    QDBusReply<MediaFileWire> reply = service.call("lookup", QString(filename.c_str()));
    if(!reply.isValid()) {
        qWarning() << "DBus call failed: " << reply.error().message() << "\n";
        return MediaFile();
    }
    return reply.value().toMediaFile();
}

std::vector<mediascanner::MediaFile> QtClient::query(const std::string &q, mediascanner::MediaType type, const mediascanner::Filter& filter) const {
    QDBusReply<QList<MediaFileWire>> reply = service.call("query", q.c_str(), (int32_t)(type), filter2vmap(filter));
    std::vector<mediascanner::MediaFile> result;
    if(!reply.isValid()) {
        qWarning() << "DBus call failed: " << reply.error().message() << "\n";
    } else {
        for(const auto &mf : reply.value()) {
            result.push_back(mf.toMediaFile());
        }
    }
    return result;
}

std::vector<mediascanner::Album> QtClient::queryAlbums(const std::string &core_term, const mediascanner::Filter &filter) const {
    QDBusReply<QList<AlbumWire>> reply = service.call("queryAlbums", core_term.c_str(), filter2vmap(filter));
    std::vector<mediascanner::Album> result;
    if(!reply.isValid()) {
        qWarning() << "DBus call failed: " << reply.error().message() << "\n";
    } else {
        for(const auto &a : reply.value()) {
            result.push_back(a.toAlbum());
        }
    }
    return result;

}

std::vector<std::string> QtClient::queryArtists(const std::string &q, const mediascanner::Filter &filter) const {

}

std::vector<mediascanner::MediaFile> QtClient::getAlbumSongs(const mediascanner::Album& album) const {

}

std::string QtClient::getETag(const std::string &filename) const {

}

std::vector<mediascanner::MediaFile> QtClient::listSongs(const mediascanner::Filter &filter) const {

}

std::vector<mediascanner::Album> QtClient::listAlbums(const mediascanner::Filter &filter) const {

}

std::vector<std::string> QtClient::listArtists(const mediascanner::Filter &filter) const {

}

std::vector<std::string> QtClient::listAlbumArtists(const mediascanner::Filter &filter) const {

}

std::vector<std::string> QtClient::listGenres(const mediascanner::Filter &filter) const {

}