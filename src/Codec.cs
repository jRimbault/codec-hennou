using System.Collections.Generic;
using System.Linq;

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
            foreach (byte c in stream)
            {
                yield return matrix.Encode[c & MASK];
                yield return matrix.Encode[(c >> 4) & MASK];
            }
        }

        private static IEnumerable<byte> Decode(Matrix matrix, IEnumerable<byte> stream)
        {
            for (var i = 0; i < stream.Count(); i += 2) {
                var a = stream.ElementAt(i);
                var b = stream.ElementAt(i + 1);
                var p1 = matrix.Decode[a];
                var p2 = matrix.Decode[b] << 4;
                yield return (byte) (p1 | p2);
            }
        }
    }
}
