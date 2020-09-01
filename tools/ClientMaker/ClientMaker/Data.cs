using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ClientMaker
{
    class Data
    {
        private string _serverAddress = null;
        private int? _port = null;
        private string _output = null;
        private string _input = null;
        private int? _webServerPort = null;

        public Data()
        {
            _input = "Client.exe";
        }
        public string ServerAddress
        {
            get
            {
                return _serverAddress;
            }

            set
            {
                _serverAddress = value;
            }
        }

        public int? Port
        {
            get
            {
                return _port;
            }

            set
            {
                _port = value;
            }
        }

        public string Output
        {
            get
            {
                return _output;
            }

            set
            {
                _output = value;
            }
        }

        public string Input
        {
            get
            {
                return _input;
            }

            set
            {
                _input = value;
            }
        }

        public bool IsValid => _port.HasValue
            && _serverAddress != null
            && _input != null
            && _output != null;

        public int? WebServerPort
        {
            get
            {
                return _webServerPort;
            }

            set
            {
                _webServerPort = value;
            }
        }
    }
}
