using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using codech.Extensions;

namespace codech
{
    class Program
    {
        delegate IEnumerable<byte> CodecWorker(IEnumerable<byte> stream);

        static void Main(string[] args)
        {
            var codec = new Codec(Matrix.From(args[0]));
            if (args[1] == "--encode" || args[1] == "-e")
            {
                Work(codec.Encode, args[2], args[3]);
            }
            else
            {
                Work(codec.Decode, args[2], args[3]);
            }
        }

        private static void Work(CodecWorker worker, string source, string dest)
        {
            var content = File.ReadAllBytes(source);
            var result = content.ToList().Chunk(ChunkSize(content.Length))
                .AsParallel()
                .AsOrdered()
                .SelectMany(c => worker(c))
                .ToArray();
            File.WriteAllBytes(dest, result);
        }

        private static int ChunkSize(int len)
        {
            if (Environment.ProcessorCount <= 0)
            {
                return len;
            }
            var res = len / Environment.ProcessorCount;
            if (res % 2 == 1) {
                return res - 1;
            }
            return res;
        }
    }
}
