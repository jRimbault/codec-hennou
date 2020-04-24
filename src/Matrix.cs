using System;
using System.IO;
using System.Linq;

namespace codech
{
    internal class Matrix
    {
        public byte[] Decode;
        public byte[] Encode;

        private Matrix(byte[] matrix)
        {
            Encode = matrix;
            Decode = new byte[256];
            for (byte i = 0; i < matrix.Length; i += 1)
            {
                Decode[matrix[i]] = i;
            }
        }

        public static Matrix From(string filename)
        {
            return new Matrix(BuildMatrixFromKey(ReadKey(filename)));
        }

        private static byte[] BuildMatrixFromKey(byte[] key)
        {
            return new byte[] {
                (byte) (key[0] ^ key[1] ^ key[2] ^ key[3]),
                key[3],
                key[2],
                (byte) (key[2] ^ key[3]),
                key[1],
                (byte) (key[1] ^ key[3]),
                (byte) (key[1] ^ key[2]),
                (byte) (key[1] ^ key[2] ^ key[3]),
                key[0],
                (byte) (key[0] ^ key[3]),
                (byte) (key[0] ^ key[2]),
                (byte) (key[0] ^ key[2] ^ key[3]),
                (byte) (key[0] ^ key[1]),
                (byte) (key[0] ^ key[1] ^ key[3]),
                (byte) (key[0] ^ key[1] ^ key[2]),
                0,
            };
        }

        private static byte[] ReadKey(string filename)
        {
            return File.ReadAllText(filename)
                       .Substring(5, 35)
                       .Split(' ')
                       .Select(bin => Convert.ToByte(bin, 2))
                       .ToArray();
        }
    }
}
