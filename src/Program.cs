using System;
using System.Collections.Generic;
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
            Codec codec = new(Matrix.FromFile(opts.KeyFile));
            if (opts.Encode)
            {
                Work(opts, codec.Encode);
            }
            else if (opts.Decode)
            {
                Work(opts, codec.Decode);
            }
        }

        private static void Work(
            Options opts,
            Func<IEnumerable<byte>, IEnumerable<byte>> worker
        )
        {
            const int MAX_SIZE = 4096;
            using (FileStream reader = new(opts.Source, FileMode.Open))
            using (FileStream writer = new(opts.Dest, FileMode.Create))
            {
                var buffer = new byte[MAX_SIZE];
                while (reader.Read(buffer, 0, buffer.Length) > 0)
                {
                    var result = worker(buffer).ToArray();
                    writer.Write(result, 0, result.Length);
                }
            }
        }
    }
}
