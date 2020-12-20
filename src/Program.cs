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
            using (FileStream innerReader = new(opts.Source, FileMode.Open))
            using (FileStream innerRriter = new(opts.Dest, FileMode.Create))
            using (BufferedStream reader = new(innerReader))
            using (BufferedStream writer = new(innerRriter))
            {
                var buffer = new byte[MAX_SIZE];
                while (reader.Read(buffer, 0, buffer.Length) > 0)
                {
                    var result = worker(buffer).ToArray();
                    writer.Write(result, 0, result.Length);
                }
            }
        }

        private static void BufferedWork(
            Options opts,
            Func<IEnumerable<byte>, IEnumerable<byte>> worker
        )
        {
            const int MAX_SIZE = 4096;
            using (FileStream innerReader = new(opts.Source, FileMode.Open))
            using (FileStream innerRriter = new(opts.Dest, FileMode.Create))
            using (BufferedStream reader = new(innerReader))
            using (BufferedStream writer = new(innerRriter))
            {
                var readBuffer = new byte[MAX_SIZE];
                var writeBuffer = new byte[opts.Encode ? MAX_SIZE * 2 : MAX_SIZE / 2];
                while (reader.Read(readBuffer, 0, readBuffer.Length) > 0)
                {
                    foreach (var (i, b) in worker(readBuffer).Enumerate())
                    {
                        writeBuffer[i] = b;
                    }
                    writer.Write(writeBuffer, 0, writeBuffer.Length);
                }
            }
        }
    }

    internal static class LinqExtension
    {
        public static IEnumerable<(int, T)> Enumerate<T>(this IEnumerable<T> enumerable)
        {
            var i = 0;
            foreach (var item in enumerable)
            {
                yield return (i, item);
                i += 1;
            }
        }
    }
}
