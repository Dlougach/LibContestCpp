#include "../NumberTheory/ModularArithmetic.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace number_theory::modular_arithmetic;

namespace NumberTheoryTests
{

TEST_CLASS(IntegerModuloTestCase)
{
public:

	TEST_METHOD(TestConstruction)
	{
		IntegerModulo<3> a = -1;
		Assert::AreEqual(static_cast<int>(a), 2);
		a = 1000LL * 1000LL * 1000LL * 1000LL * 1000LL * 1000LL;
		Assert::AreEqual(static_cast<int>(a), 1); //
	}

	TEST_METHOD(TestAddition)
	{
		using Int = IntegerModulo<1000 * 1000 * 1000 + 7>;
		Int a, b;
		a = -1;
		b = -2;
		Assert::AreEqual(Int(-3), a + b);
	}

	TEST_METHOD(TestSubtraction)
	{
		using Int = IntegerModulo<1000 * 1000 * 1000 + 7>;
		Int a, b;
		a = -1;
		b = -2;
		Assert::AreEqual(Int(1), a - b);
	}

	TEST_METHOD(TestMultiplication)
	{
		using Int = IntegerModulo<1000 * 1000 * 1000 + 7>;
		Int a, b;
		a = -2;
		b = -3;
		Assert::AreEqual(Int(6), a * b);
	}

	TEST_METHOD(TestDivision)
	{
		using Int = IntegerModulo<1000 * 1000 * 1000 + 7>;
		Int a, b;
		a = -2;
		b = -3;
		Assert::AreEqual(a, (a / b) * b);
	}

};

}