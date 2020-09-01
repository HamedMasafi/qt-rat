using System;

namespace Client
{
    public class Data
    {
#if DEBUG
        public static string Server = "127.0.0.1";
        public static int NeuronPort = 5100;
        public static int WebServerPort = 80;
        public static string Arch = "";
        public static int CreateIniFile = int.MinValue;
        public static int Ready = 1;
#else
        public static string Server = "127.0.0.1";
        public static int NeuronPort = int.MaxValue;
        public static int WebServerPort = int.MaxValue;
        public static string Arch = "";
        public static int CreateIniFile = int.MinValue;
        public static int Ready = int.MinValue;
#endif

        internal static string Stringify()
        {
            return $"Server={Server}, NeuronPort={NeuronPort}, WebServerPort={WebServerPort}, CreateIniFile={CreateIniFile}, Arch={Arch}, Ready={Ready}";
        }
    }
}
