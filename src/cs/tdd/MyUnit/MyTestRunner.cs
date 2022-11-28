using System;
using System.Linq;
using System.Reflection;
using MyUnit.Attributes;

namespace MyUnit
{
    public static class MyTestRunner
    {
        public static void RunForType(
            Type type,
            Action<string> printResult)
        {
            var methods = type
                .GetMethods()
                .Where(o => o.GetCustomAttribute<MyFactAttribute>() != null)
                .ToArray();

            if (methods.Any())
            {
                var instance = Activator.CreateInstance(type);

                foreach (var method in methods)
                {
                    method.Invoke(instance, new object[0]);

                    var methodName = method.Name;

                    if (!MyAssert.AssertWasInvoked)
                        throw new InvalidOperationException($"{methodName} не содержит проверок");

                    printResult?.Invoke(MyAssert.LastRunWasSuccessful
                        ? $"{methodName}: прошёл"
                        : $"{methodName}: провален");

                    MyAssert.ClearLastRunResult();
                }
            }
        }
    }
}
