using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using codech.Extensions;

namespace codech
{
    class Program
    {
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

        private static void Work(Func<IEnumerable<byte>, IEnumerable<byte>> worker, string source, string dest)
        {
            var content = File.ReadAllBytes(source);
            var chunkSize = ChunkSize(content.Length, Environment.ProcessorCount);
            var result = content.Chunk(chunkSize)
                // .AsParallel()
                // .AsOrdered()
                .SelectMany(worker)
                .ToArray();
            File.WriteAllBytes(dest, result);
        }

        private static int ChunkSize(int len, int max)
        {
            if (max <= 0)
            {
                return len;
            }
            return (len / max) & 0xfffe;
        }
    }
}
