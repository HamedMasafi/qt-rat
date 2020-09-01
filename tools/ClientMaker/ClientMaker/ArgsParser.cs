using Mono.Options;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ClientMaker
{
    class ArgsParser
    {
        string[] args;
        Data _data = new Data();
        
        public ArgsParser(string[] args)
        {
            this.args = args;
        }

        internal Data Data
        {
            get
            {
                return _data;
            }

            set
            {
                _data = value;
            }
        }

        public bool ParseArgs()
        {
            var p = new OptionSet() {
                {"s|server=", "Server address.", v => Data.ServerAddress = v },
                {"p|port=", "Port number.", (int v) => Data.Port = v },
                {"w|webport=", "Web server port.", (int v) => Data.WebServerPort = v },
                {"o|output=", "Output file name.", v => Data.Output = v },
                {"i|input=", "Input file name.", v => Data.Input = v },
            };

            List<string> extra;
            try
            {
                extra = p.Parse(args);
                foreach (var e in extra)
                    Console.Write(p + "\t");
                return true;
            }
            catch (OptionException e)
            {
                Console.Write("ClientMaker: ");
                Console.WriteLine(e.Message);
                Console.WriteLine("Try `ClientMaker --help' for more information.");

                p.WriteOptionDescriptions(Console.Out);
                return false;
            }
        }
    }
}
