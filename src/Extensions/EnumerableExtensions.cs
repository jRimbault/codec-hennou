using System;
using System.Collections.Generic;

namespace codech.Extensions
{
    public static class EnumerableExtensions
    {
        public static IEnumerable<IList<T>> Chunk<T>(this List<T> locations, int nSize=30)
        {
            for (int i = 0; i < locations.Count; i += nSize)
            {
                yield return locations.GetRange(i, Math.Min(nSize, locations.Count - i));
            }
        }
    }
}
