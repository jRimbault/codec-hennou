using System.Collections.Generic;

namespace codech
{
    internal class Codec
    {
        private const byte Mask = 0x0f;
        private readonly Matrix _matrix;

        public Codec(Matrix matrix)
        {
            _matrix = matrix;
        }

        public IEnumerable<byte> Encode(IEnumerable<byte> stream)
        {
            foreach (byte b in stream)
            {
                yield return _matrix.Encode[b & Mask];
                yield return _matrix.Encode[(b >> 4) & Mask];
            }
        }

        public IEnumerable<byte> Decode(IEnumerable<byte> stream)
        {
            var couple = new List<byte>(2);
            foreach (byte b in stream)
            {
                couple.Add(b);
                if (couple.Count == 2)
                {
                    byte p1 = _matrix.Decode[couple[0]];
                    var p2 = (byte) (_matrix.Decode[couple[1]] << 4);
                    yield return (byte) (p1 | p2);
                    couple.Clear();
                }
            }
        }
    }
}
