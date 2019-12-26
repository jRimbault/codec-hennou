using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace codech
{
    class Matrix
    {
        public IList<byte> Encode { get; }
        public IList<byte> Decode { get; }

        private Matrix(byte[] matrix)
        {
            this.Encode = matrix;
            this.Decode = new byte[255];
            for (byte i = 0; i < matrix.Length; i += 1) {
                this.Decode[matrix[i]] = i;
            }
        }

        public static Matrix From(string filename)
        {
            return new Matrix(BuildMatrixFromKey(ReadKey(filename)));
        }

        private static byte[] BuildMatrixFromKey(byte[] key)
        {
            var matrix = new byte[16];
            matrix[15] = 0;
            matrix[1] = key[3];
            matrix[2] = key[2];
            matrix[3] = (byte) (key[2] ^ key[3]);
            matrix[4] = key[1];
            matrix[5] = (byte) (key[1] ^ key[3]);
            matrix[6] = (byte) (key[1] ^ key[2]);
            matrix[7] = (byte) (key[1] ^ key[2] ^ key[3]);
            matrix[8] = key[0];
            matrix[9] = (byte) (key[0] ^ key[3]);
            matrix[10] = (byte) (key[0] ^ key[2]);
            matrix[11] = (byte) (key[0] ^ key[2] ^ key[3]);
            matrix[12] = (byte) (key[0] ^ key[1]);
            matrix[13] = (byte) (key[0] ^ key[1] ^ key[3]);
            matrix[14] = (byte) (key[0] ^ key[1] ^ key[2]);
            matrix[0] = (byte) (key[0] ^ key[1] ^ key[2] ^ key[3]);
            return matrix;
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
