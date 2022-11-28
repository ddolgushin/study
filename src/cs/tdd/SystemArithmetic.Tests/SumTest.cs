using MyUnit;
using MyUnit.Attributes;

namespace SystemArithmetic.Tests
{
    public class SumTest
    {
        [MyFact]
        public void OnePlusOne_EqualsTwo()
        {
            // Arrange.
            const int a = 1;
            const int b = 1;
            const int expected = 2;

            // Act.
            const int result = a + b;

            // Assert.
            MyAssert.Equal(expected, result);
        }

        [MyFact]
        public void TwoTimesTwo_EqualsFour()
        {
            // Arrange.
            const int a = 2;
            const int b = 2;
            const int expected = 4;

            // Act.
            const int result = a * b;

            // Assert.
            MyAssert.Equal(expected, result);
        }

        [MyFact]
        public void FourByTwo_EqualsTwo()
        {
            // Arrange.
            const int a = 4;
            const int b = 2;
            const int expected = 2;

            // Act.
            const int result = a / b;

            // Assert.
            MyAssert.Equal(expected, result);
        }
    }
}
