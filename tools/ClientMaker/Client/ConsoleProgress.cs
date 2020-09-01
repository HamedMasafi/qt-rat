using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Client
{
    class ConsoleProgress
    {
        int _total;
        int _value;
        public int Total
        {
            get
            {
                return _total;
            }

            set
            {
                _total = value;
                Draw();
            }
        }

        public int Value
        {
            get
            {
                return _value;
            }

            set
            {
                _value = value;
                Draw();
            }
        }

        public string Title { get; set; }
        public bool ShowPercent { get; set; }

        public ConsoleColor ProgressColor { get; set; } = ConsoleColor.Green;
        public ConsoleColor BackColor { get; set; } = ConsoleColor.Black;

        public ConsoleProgress()
        {

        }
        public ConsoleProgress(int max)
        {
            Total = max;
        }
        public void Draw()
        {
            Console.CursorLeft = 0;

            if (!string.IsNullOrWhiteSpace(Title))
                Console.Write(Title + " ");

            Console.Write("["); //start
            
            var v = (int)(((float)_value / (float)_total) * 50.0f);
            Console.BackgroundColor = ConsoleColor.Green;
            for (int i = 0; i <= v; i++)
                Console.Write(" ");

            Console.BackgroundColor = ConsoleColor.Black;
            for (int i = (int)v + 1; i <= 50; i++)
                Console.Write(" ");

            Console.Write("]"); //end

            if (ShowPercent)
                Console.Write(" {0}%  ", v * 2);
            else
                Console.Write(" {0}/{1}  ", _value, _total); //blanks at the end remove any excess

            if (_value >= _total)
                Console.WriteLine();
        }

        static int last_msg_size = 0;
        public static void ShowMessage(string msg)
        {
            last_msg_size = msg.Length + 3;
            Console.Write(msg + "...");
        }
        public static void ShowMessageResult(bool r)
        {
            for (int i = last_msg_size; i < 53; i++)
                Console.Write(" ");

            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("[");
            if (r)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write("OK");
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.Write("Error");
            }

            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("]");
        }
    }
}
