#include "../NumberTheory/BigInteger.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace number_theory::big_integer;


namespace NumberTheoryTests
{

TEST_CLASS(BigIntegerTests)
{
public:
	TEST_METHOD(TestBigIntegerMultiplication)
	{
		BigUint l = 1000000000000LL; // 10^12
		BigUint r("1000000000000");
		Assert::AreEqual(std::string("1000000000000000000000000"), (l * r).str());
	}
};

}  // namespace NumberTheoryTes