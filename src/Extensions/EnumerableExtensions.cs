using System;
using System.Collections.Generic;

namespace codech.Extensions
{
    public static class EnumerableExtensions
    {
        public static IEnumerable<IEnumerable<T>> Chunk<T>(this IEnumerable<T> source, int len)
        {
            if (len == 0)
            {
                throw new ArgumentNullException();
            }
            var enumerator = source.GetEnumerator();
            while (enumerator.MoveNext())
            {
                yield return Take(enumerator.Current, enumerator, len);
            }
        }

        private static IEnumerable<T> Take<T>(T head, IEnumerator<T> tail, int len)
        {
            while (true)
            {
                yield return head;
                if (--len == 0)
                {
                    break;
                }
                if (tail.MoveNext())
                {
                    head = tail.Current;
                }
                else
                {
                    break;
                }
            }
        }
    }
}
