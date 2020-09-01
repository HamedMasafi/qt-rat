#ifndef CLIENTSMODEL_H
#define CLIENTSMODEL_H

#include <QAbstractListModel>
#include <QPixmap>

namespace Neuron {
class Server;
class Peer;
}
class AbstractClient;
class ClientsModel : public QAbstractListModel
{
    Q_OBJECT
    struct Data {
        AbstractClient *client;
        QString version;
        QString platform;
        QString kernelType;
        QString buildAbi;
        QString productType;
        QString productVersion;
        QString prettyProductName;
        QStringList features;
        QPixmap thumbnail;
    };

    QList<Data*> _list;
    bool _showThumbnail;
    int _thumbnailSize;

public:
    ClientsModel(Neuron::Server *server);
    AbstractClient *client(const QModelIndex &index);
    QStringList features(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool showThumbnail() const;

    int thumbnailSize() const;

public slots:
    void setShowThumbnail(bool showThumbnail);
    void setThumbnailSize(int thumbnailSize);

private slots:
    void server_peerConnected(Neuron::Peer *peer);
    void server_peerDisconnected(Neuron::Peer *peer);

signals:
    void peerDisconnected();
};

#endif // CLIENTSMODEL_H
