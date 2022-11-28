using System;
using SystemArithmetic.Tests;
using MyUnit;

namespace MyUnitEntryPoint
{
    class Program
    {
        static void Main(string[] args)
        {
            var targetType = typeof(SumTest);

            MyTestRunner.RunForType(targetType, Console.WriteLine);
        }
    }
}
