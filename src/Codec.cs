using System.Collections.Generic;
using System.Linq;
using codech.Extensions;
using System;

namespace codech
{
    class Codec
    {
        private static readonly byte MASK = 0x0f;
        private readonly Matrix matrix;

        public Codec(Matrix matrix)
        {
            this.matrix = matrix;
        }

        public IEnumerable<byte> Encode(IEnumerable<byte> stream)
        {
            return Encode(this.matrix, stream);
        }

        public IEnumerable<byte> Decode(IEnumerable<byte> stream)
        {
            return Decode(this.matrix, stream);
        }

        private static IEnumerable<byte> Encode(Matrix matrix, IEnumerable<byte> stream)
        {
            return stream.SelectMany(c => new byte[]
                {
                    matrix.key[c & MASK],
                    matrix.key[(c >> 4) & MASK],
                }
            );
        }

        private static IEnumerable<byte> Decode(Matrix matrix, IEnumerable<byte> stream)
        {
            return stream.Chunk(2).Select(cc => {
                byte[] chars = cc.ToArray();
                var pos1 = (byte) matrix.key.IndexOf(chars[0]);
                var pos2 = (byte) ((byte) (matrix.key.IndexOf(chars[1])) << 4);
                return (byte) (pos1 | pos2);
            });
        }
    }
}
