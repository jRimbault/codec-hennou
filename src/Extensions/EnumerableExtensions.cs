using System;
using System.Collections.Generic;

namespace codech.Extensions
{
    public static class EnumerableExtensions
    {
        public static IEnumerable<IList<T>> Chunk<T>(this List<T> list, int size = 30)
        {
            for (var i = 0; i < list.Count; i += size)
            {
                yield return list.GetRange(i, Math.Min(size, list.Count - i));
            }
        }
    }
}
