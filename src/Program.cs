using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using codech.Extensions;

namespace codech
{
    internal class Program
    {
        private delegate IEnumerable<byte> CodecWorker(IEnumerable<byte> stream);

        private static void Main(string[] args)
        {
            var codec = new Codec(Matrix.From(args[0]));
            bool timings = args.Any(a => a == "-t" || a == "--timings");
            if (args[1] == "--encode" || args[1] == "-e")
            {
                Work(codec.Encode, args[2], args[3], timings);
            }
            else
            {
                Work(codec.Decode, args[2], args[3], timings);
            }
        }

        private static void Work(CodecWorker worker, string source, string dest, bool timings)
        {
            var readStart = DateTime.Now;
            var content = File.ReadAllBytes(source);
            var readEnd = DateTime.Now - readStart;
            var workStart = DateTime.Now;
            var result = worker(content).ToArray();
            // byte[] result = content.ToList().Chunk(ChunkSize(content.Length))
            //                        .AsParallel()
            //                        .AsOrdered()
            //                        .SelectMany(c => worker(c))
            //                        .ToArray();
            var workEnd = DateTime.Now - workStart;
            var writeStart = DateTime.Now;
            File.WriteAllBytes(dest, result);
            var writeEnd = DateTime.Now - writeStart;
            if (timings) {
                Console.WriteLine($"{readEnd.TotalSeconds} {workEnd.TotalSeconds} {writeEnd.TotalSeconds}");
            }
        }

        private static int ChunkSize(int len)
        {
            if (Environment.ProcessorCount <= 0)
            {
                return len;
            }
            int res = len / Environment.ProcessorCount;
            if (res % 2 == 1) {
                return res - 1;
            }
            return res;
        }
    }
}
