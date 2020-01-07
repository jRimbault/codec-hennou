using System;
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
            var list = new List<byte>();
            for (var i = 0; i < stream.Count(); i += 1)
            {
                var e = stream.ElementAt(i);
                list.Add(this.matrix.Encode[e & MASK]);
                list.Add(this.matrix.Encode[(e >> 4) & MASK]);
            }
            return list;
        }

        public IEnumerable<byte> Decode(IEnumerable<byte> stream)
        {
            var list = new List<byte>();
            for (var i = 0; i < stream.Count(); i += 2) {
                var a = stream.ElementAt(i);
                var b = stream.ElementAt(i + 1);
                var p1 = this.matrix.Decode[a];
                var p2 = this.matrix.Decode[b] << 4;
                list.Add((byte) (p1 | p2));
            }
            return list;
        }
    }
}
