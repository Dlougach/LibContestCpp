#include "../NumberTheory/ModularArithmetic.h"
#include "CppUnitTest.h"
#include <random>
#include <vector>

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

	TEST_METHOD(TestFastDotProduct)
	{
		using Int = IntegerModulo<1000 * 1000 * 1000 + 7>;
		std::vector<Int> list1, list2;
		std::mt19937 gen;
		std::uniform_int_distribution<> dis(0, Int::MOD - 1);
		const int numValues = 10000;
		Int expectedResult = 0;
		for (int i = 0; i < numValues; ++i) {
			list1.push_back(dis(gen));
			list2.push_back(dis(gen));
			expectedResult += list1.back() * list2.back();
		}
		Assert::AreEqual(expectedResult, FastDotProduct(list1.begin(), list2.begin(), numValues));
	}
};

}