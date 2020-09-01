using System;

namespace ClientMaker
{
    class Program
    {
        //ClientMaker.exe -p 4 -a 127.0.0.1 -i  Client.exe -o client2.exe
        static void Main(string[] args)
        {
            var p = new ArgsParser(args);

            if (!p.ParseArgs())
            {
                Console.WriteLine("Error: Unable to parse parameteres");
                return;
            }

            var data = p.Data;

            if (!data.IsValid)
            {
                Console.WriteLine("Error: Invalid parameteres");
                return;
            }

            var builder = new Builder(data);
            var b = builder.Modify();

            if (b)
                Console.WriteLine("*OK*");
            else
                Console.WriteLine("Error");
        }
    }
}
