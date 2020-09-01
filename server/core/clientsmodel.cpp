#include "clientsmodel.h"
#include "../common/abstractclient.h"

#include <QIcon>
#include <Server>
#include <QHostAddress>
#include <QThread>

bool ClientsModel::showThumbnail() const
{
    return _showThumbnail;
}

void ClientsModel::setShowThumbnail(bool showThumbnail)
{
    _showThumbnail = showThumbnail;
    emit dataChanged(index(0, 0), index(_list.count() - 1, 0));
}

int ClientsModel::thumbnailSize() const
{
    return _thumbnailSize;
}

void ClientsModel::setThumbnailSize(int thumbnailSize)
{
    _thumbnailSize = thumbnailSize;
    emit dataChanged(index(0, 0), index(_list.count() - 1, 0));
}

ClientsModel::ClientsModel(Neuron::Server *server) : QAbstractListModel(server), _showThumbnail(true), _thumbnailSize(128)
{
    connect(server, &Neuron::Server::peerConnected, this, &ClientsModel::server_peerConnected, Qt::QueuedConnection);
    connect(server, &Neuron::Server::peerDisconnected, this, &ClientsModel::server_peerDisconnected);
}

AbstractClient *ClientsModel::client(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;
    if (index.row() < 0 || index.row() > _list.count() - 1)
        return nullptr;
    auto t = _list.at(index.row())->client;
    if (t)
        return qobject_cast<AbstractClient*>(t);
    return nullptr;
}

QStringList ClientsModel::features(const QModelIndex &index) const
{
    if (!index.isValid())
        return QStringList();
    if (index.row() < 0 || index.row() > _list.count() - 1)
        return QStringList();
    return _list.at(index.row())->features;
}

QVariant ClientsModel::data(const QModelIndex &index, int role) const
{
    if (_showThumbnail && role == Qt::DecorationRole && index.column() == 0) {
        auto d = _list.at(index.row());
        return d->thumbnail.scaled(_thumbnailSize, _thumbnailSize, Qt::KeepAspectRatio);
    }
    if (role == Qt::DecorationRole && index.column() == 1) {
        auto d = _list.at(index.row());
        if (d->platform == "Windows")
            return QIcon(":/icons/windows.png");
        if (d->platform == "Linux")
            return QIcon(":/icons/linux.png");
        return QVariant();
    }

    if (role == Qt::ToolTipRole) {
        auto d = _list.at(index.row());
        return QString(
            "RAT Version: <b>%1</b><br />"
            "Platform: <b>%2</b><br />"
            "Kernel type: <b>%3</b><br />"
            "Build abi: <b>%4</b><br />"
            "Product type: <b>%5</b><br />"
            "Product version: <b>%6</b><br />"
            "Pretty product name: <b>%7</b><br />")
            .arg(d->version, d->platform, d->kernelType, d->buildAbi, d->productType, d->productVersion, d->prettyProductName);
    }

    if (role == Qt::DisplayRole) {
        auto d = _list.at(index.row());
        switch (index.column()) {
        case 0: {
            auto a = d->client->hub()->peerAddress();
//            if (a.isSiteLocal() || a.isLinkLocal())
//                return "Local machine";

            if (a.isNull())
                return "Null";
            if (a.protocol() == QAbstractSocket::IPv4Protocol) {
                auto ip = a.toIPv4Address();
                return QString("%1.%2.%3.%4")
                    .arg((ip & 0xff000000) >> 24)
                    .arg((ip & 0x00ff0000) >> 16)
                    .arg((ip & 0x0000ff00) >> 8)
                    .arg((ip & 0x000000ff));
            }
            return a.toString();
        }

        case 1:
            return d->platform;

        case 2:
            return d->version;
        }
    }
    return QVariant();
    //Neuron::ServerModel::data(index, role);
}

void ClientsModel::server_peerConnected(Neuron::Peer *peer)
{
    qDebug() << Q_FUNC_INFO;
    auto cl = qobject_cast<AbstractClient*>(peer);
    if (cl) {
        qDebug() << "getting info from new client";
//        QThread::sleep(3);
        cl->hub()->flushSocket();
        cl->info()->then([this,cl](QVariant v){
            qDebug() << "info recived";
            auto map = v.toMap();
            beginInsertRows(QModelIndex(), _list.count(), _list.count());
            auto data = new Data;
            data->client = cl;
            data->version = map.value("version").toString();
            data->platform = map.value("platform").toString();
            data->kernelType = map.value("kernelType").toString();
            data->buildAbi = map.value("buildAbi").toString();
            data->productType = map.value("productType").toString();
            data->productVersion = map.value("productVersion").toString();
            data->prettyProductName = map.value("prettyProductName").toString();

            auto l = map.value("features").toList();
            foreach (QVariant v, l)
                data->features.append(v.toString());

            int i = _list.count();
            cl->screenshot()->then([this, data, i](QImage img){
               data->thumbnail = QPixmap::fromImage(img);
               emit dataChanged(index(i, 0), index(i, 0));
            });
            _list.append(data);
            endInsertRows();
        });
    } else {
        qDebug() << "Invalid client";
    }
}

void ClientsModel::server_peerDisconnected(Neuron::Peer *peer)
{
    for (int i = 0; i < _list.count(); ++i) {
        if (_list.at(i)->client == peer) {
            beginRemoveRows(QModelIndex(), i, i);
            _list.removeAt(i);
            endRemoveRows();
            return;
        }
    }
    emit peerDisconnected();
}

int ClientsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _list.count();
}

int ClientsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant ClientsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return section + 1;

    switch (section) {
        case 0: return "Address";
        case 1: return "Platform";
        case 2: return "Version";
        case 3: return "Kernel type";
        case 4: return "Build abi";
        case 5: return "Product type";
        case 6: return "Product version";
        case 7: return "Pretty product name";
    }
    return QVariant();
}
