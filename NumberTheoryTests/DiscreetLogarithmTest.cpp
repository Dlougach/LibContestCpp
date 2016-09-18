#include "../NumberTheory/DiscreetLogarithm.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace number_theory::discreet_logarithm;


namespace NumberTheoryTests
{

TEST_CLASS(FindPrimitiveRootTests)
{
public:
	TEST_METHOD(TestFindsRootForLargePrime)
	{
		auto root = FindPrimitiveRoot<1000000007>();
		Assert::AreNotEqual(0, static_cast<int>(root));
	}
};

}  // namespace NumberTheoryTests