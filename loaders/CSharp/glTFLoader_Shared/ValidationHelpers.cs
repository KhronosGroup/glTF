using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace glTFLoader.Shared
{
    internal static class ValidationHelpers
    {
        public static void ValidateString(string s, int minLength, int maxLength)
        {
            if (minLength != -1 && s.Length < minLength)
            {
                throw new InvalidDataException("The length of the string is shorter than the minimum");
            }
            if (maxLength != -1 && s.Length > maxLength)
            {
                throw new InvalidDataException("The length of the string is longer than the maximum");
            }
        }

        public static float ToFloat(this object value)
        {
            var type = value.GetType();
            if (type == typeof(float))
            {
                return (float)value;
            }
            if (type == typeof(double))
            {
                return (float)(double)value;
            }
            if (type == typeof(long))
            {
                return (float)(long)value;
            }
            if (type == typeof(int))
            {
                return (float)(int)value;
            }
            throw new NotImplementedException();
        }

        public static void CheckRange<t_value>(
            t_value value,
            t_value minimum,
            t_value maximum,
            bool hasMinimum,
            bool hasMaximum,
            bool isExclusiveMinimum,
            bool isExclusiveMaximum) where t_value : IComparable
        {
            if (hasMinimum)
            {
                if (value.CompareTo(minimum) < 0)
                {
                    throw new InvalidDataException("The number is smaller than minimum");
                }
                if (isExclusiveMinimum && value.CompareTo(minimum) == 0)
                {
                    throw new InvalidDataException("The number is equal to the minimum and the minumum is exlcusive");
                }
            }
            if (hasMaximum)
            {
                if (value.CompareTo(maximum) > 0)
                {
                    throw new InvalidDataException("The number is larger than maximum");
                }
                if (isExclusiveMaximum && value.CompareTo(maximum) == 0)
                {
                    throw new InvalidDataException("The number is equal to the maximum and the maximum is exlcusive");
                }
            }
        }
    }
}
