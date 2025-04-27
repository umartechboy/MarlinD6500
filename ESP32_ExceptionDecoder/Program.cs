using System.Diagnostics;
using System.IO.Ports;
using System.Reflection;

namespace ESP32_ExceptionDecoder
{
    internal class Program
    {
        static string elf = ".pio/build/D8500/firmware.elf";
        static string traceDecode(string trace)
        {
            var addr2Line = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".platformio/packages/toolchain-xtensa-esp32/bin/xtensa-esp32-elf-addr2line");
            var p = new Process() { StartInfo = new ProcessStartInfo(addr2Line, "-e " + elf + " -p ESP32  " + trace) { UseShellExecute = false, RedirectStandardOutput = true } };
            p.Start();
            return p.StandardOutput.ReadToEnd();
        }
        static void WriteLine(string message, params object[] args)
        {
            Write(message + "\r\n", args);
        }
        static void Write(char chr)
        {
            Write(chr.ToString());
        }
        static void Write(string message, params object[] args)
        {
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write(message, args);
        }
        static void WriteErrorLine(string message, params object[] args)
        {
            WriteError(message + "\r\n", args);
        }
        static void WriteTraceLine(string message)
        {
            var fName = message.Substring(0, message.LastIndexOf(":"));
            var dir = Path.GetDirectoryName(fName);
            dir = dir.TrimStart('?', '\r', '\n', ':', '0');
            
            var file = Path.GetFileName(fName);
            var line = message.Substring(message.LastIndexOf(":") + 1);
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.BackgroundColor = ConsoleColor.Black;
            Console.Write(dir);

            Console.ForegroundColor = ConsoleColor.White;
            Console.BackgroundColor = ConsoleColor.Black;
            Console.Write("/");

            Console.ForegroundColor = ConsoleColor.White;
            Console.BackgroundColor = ConsoleColor.DarkBlue;
            Console.Write(file);

            Console.ForegroundColor = ConsoleColor.White;
            Console.BackgroundColor = ConsoleColor.Black;
            Console.Write(":");

            Console.ForegroundColor = ConsoleColor.White;
            Console.BackgroundColor = ConsoleColor.DarkRed;
            Console.Write(line.Trim());

            Console.ForegroundColor = ConsoleColor.White;
            Console.BackgroundColor = ConsoleColor.Black;
            Console.WriteLine();
        }
        static void WriteError(string message, params object[] args)
        {
            Console.ForegroundColor = ConsoleColor.Black;
            Console.BackgroundColor = ConsoleColor.Red;
            Console.Write(message, args);
        }
        static void Main(string[] args)
        {
            Console.WriteLine("ESP32 Exception Decoder");
            var name = SerialPort.GetPortNames().Last();
            var baud = 250000;
            Console.WriteLine("Opening {0} at {1}", name, baud);
            var sp = new SerialPort(name, baud);
            List<char> line = new();
            int restartCount = 0;
            bool stopPrinting = false;
            void newLine(string line)
            {
                if (line.StartsWith("ELF file SHA256"))
                    restartCount++;
                else if (stopPrinting)
                    return;
                if (line.StartsWith("PC"))
                {
                    var pc = line.Split(new char[] { '\t', ' ', ':' }, StringSplitOptions.RemoveEmptyEntries)[1];
                    Console.WriteLine();
                    WriteErrorLine("Exception occured at: ");
                    WriteTraceLine(traceDecode(pc).Trim());
                }
                else if (line.StartsWith("Guru Meditation"))
                {
                    Console.WriteLine();
                    WriteErrorLine(line);
                }
                else if (line.StartsWith("Backtrace: "))
                {
                    Console.WriteLine();
                    WriteErrorLine("Decoded Backtrace: ");
                    var traces = line.Split(' ').ToList().Skip(1);
                    foreach(var trace in traces)
                    {
                        var decode = traceDecode(trace).Trim();
                        WriteTraceLine(decode);
                    }

                }
                if (restartCount >= 1)
                {
                    if (restartCount == 1)
                        Console.WriteLine();
                    stopPrinting = true;
                    WriteError("\rBoot loop detected: {0}", restartCount);
                    return;
                }
            }
            sp.DataReceived += (sender, e) =>
        {
            while (sp.BytesToRead > 0)
            {
                var bytes = new byte[sp.BytesToRead];
                sp.Read(bytes, 0, bytes.Length);
                foreach (var bRead in bytes)
                {
                    if (bRead == '\n')
                    {
                        var l = string.Join("", line);
                        line.Clear();
                        newLine(l);
                    }
                    else
                        line.Add((char)bRead);
                    if (!stopPrinting)
                        Write((char)bRead);
                }
            }
        };
            sp.Open();
            Console.WriteLine("Opened successfully");
            var inStream = Console.OpenStandardInput();
            StreamReader sr = new (inStream);
            var rLine = sr.ReadLine();
            while (true)
            {
                sp.WriteLine(rLine);        
                rLine = sr.ReadLine();
            }
        }

    }
}
