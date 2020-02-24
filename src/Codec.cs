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
            var couple = new Queue<byte>(2);
            foreach (byte b in stream)
            {
                couple.Enqueue(b);
                if (couple.Count == 2)
                {
                    byte p1 = _matrix.Decode[couple.Dequeue()];
                    var p2 = (byte) (_matrix.Decode[couple.Dequeue()] << 4);
                    yield return (byte) (p1 | p2);
                }
            }
        }
    }
}
