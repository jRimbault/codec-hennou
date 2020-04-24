using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using CommandLine;

namespace codech
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            Parser.Default.ParseArguments<Options>(args).WithParsed(Run);
        }

        private static void Run(Options opts)
        {
            var codec = new Codec(Matrix.From(opts.KeyFile));
            if (opts.Encode)
            {
                Work(codec.Encode, opts);
            }
            else if (opts.Decode)
            {
                Work(codec.Decode, opts);
            }
        }

        private static void Work(
            Func<IEnumerable<byte>, IEnumerable<byte>> worker,
            Options opts
        )
        {
            DateTime readStart = DateTime.Now;
            byte[] content = File.ReadAllBytes(opts.Source);
            TimeSpan readEnd = DateTime.Now - readStart;
            DateTime workStart = DateTime.Now;
            byte[] result = worker(content).ToArray();
            // byte[] result = content.ToList().Chunk(ChunkSize(content.Length))
            //                        .AsParallel()
            //                        .AsOrdered()
            //                        .SelectMany(c => worker(c))
            //                        .ToArray();
            TimeSpan workEnd = DateTime.Now - workStart;
            DateTime writeStart = DateTime.Now;
            File.WriteAllBytes(opts.Dest, result);
            TimeSpan writeEnd = DateTime.Now - writeStart;
            if (opts.Timings)
            {
                string[] formattedTimes = new[]
                {
                    readEnd,
                    workEnd,
                    writeEnd
                }.Select(d => d.TotalSeconds.ToString(CultureInfo.InvariantCulture)).ToArray();
                Console.WriteLine(string.Join(" ", formattedTimes));
            }
        }

        private static int ChunkSize(int len)
        {
            if (Environment.ProcessorCount <= 0)
            {
                return len;
            }

            int res = len / Environment.ProcessorCount;
            if (res % 2 == 1 && res != 1)
            {
                return res - 1;
            }

            return res;
        }
    }
}
