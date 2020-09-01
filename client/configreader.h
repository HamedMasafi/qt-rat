#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QString>

namespace Neuron {
    class ClientHub;
}
class ConfigReader
{
    Neuron::ClientHub *_clientHub;
    quint16 _port;
    QString _server;

    bool _showMessageBox;
    QString _messageBoxTitle;
    QString _messageBoxBody;
    QString _installPath;
    QString _appName;

public:
    enum ReadType {
        InsideFile = 1,
        SettingFile = 2,
        BroadCasting = 4,
        All = InsideFile | SettingFile | BroadCasting
    };
    Q_DECLARE_FLAGS(ReadTypes, ReadType)


    ConfigReader();
    bool read(const ReadTypes &t = All);

    quint16 port() const;
    QString server() const;
    QString messageHeader() const;
    QString messageBody() const;
    QString installPath() const;
    bool showMessageBox() const;
    QString messageBoxTitle() const;
    QString messageBoxBody() const;
    QString appName() const;

    Neuron::ClientHub *clientHub() const;
    void setClientHub(Neuron::ClientHub *clientHub);

    void print() const;
private:
    bool readConfig();
    bool findInBroadcast();
    bool readInFile();
    void createSampleData();
};

#endif // CONFIGREADER_H
