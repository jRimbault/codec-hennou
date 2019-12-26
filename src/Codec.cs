using System.Collections.Generic;

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
                yield return matrix.Value[c & MASK];
                yield return matrix.Value[(c >> 4) & MASK];
            }
        }

        private static IEnumerable<byte> Decode(Matrix matrix, IEnumerable<byte> stream)
        {
            var pair = new List<byte>(2);
            foreach (var c in stream)
            {
                pair.Add(c);
                if (pair.Count == 2)
                {
                    var pos1 = matrix.Value.IndexOf(pair[0]);
                    var pos2 = matrix.Value.IndexOf(pair[1]) << 4;
                    pair.Clear();
                    yield return (byte) (pos1 | pos2);
                }
            }
        }
    }
}
