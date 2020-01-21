using System.Collections.Generic;
using System.Linq;

namespace codech
{
    internal class Codec
    {
        private const byte Mask = 0x0f;
        private readonly Matrix _matrix;

        public Codec(Matrix matrix)
        {
            this._matrix = matrix;
        }

        public IEnumerable<byte> Encode(IEnumerable<byte> stream)
        {
            var list = new List<byte>();
            byte[] bytes = stream as byte[] ?? stream.ToArray();
            for (var i = 0; i < bytes.Length; i += 1)
            {
                byte e = bytes[i];
                list.Add(this._matrix.Encode[e & Mask]);
                list.Add(this._matrix.Encode[(e >> 4) & Mask]);
            }
            return list;
        }

        public IEnumerable<byte> Decode(IEnumerable<byte> stream)
        {
            var list = new List<byte>();
            byte[] bytes = stream as byte[] ?? stream.ToArray();
            for (var i = 0; i < bytes.Count(); i += 2) {
                byte a = bytes[i];
                byte b = bytes[i];
                byte p1 = this._matrix.Decode[a];
                var p2 = (byte) (this._matrix.Decode[b] << 4);
                list.Add((byte) (p1 | p2));
            }
            return list;
        }
    }
}
