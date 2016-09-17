#include "../NumberTheory/Factorization.h"
#include "CppUnitTest.h"
#include <random>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace number_theory::factorization;

namespace NumberTheoryTests {

TEST_CLASS(FactorizationTestCase)
{
public:

	TEST_METHOD(TestFactorization1e9)
	{
		auto res = Factorize(1000000006);
		int product = 1;
		for (auto pp : res) {
			Assert::IsTrue(IsPrime(pp.first));
			for (unsigned i = 0; i < pp.second; ++i) {
				product *= pp.first;
			}
		}
		Assert::AreEqual(1000000006, product);
	}
};

}  // namespace NumberTheoryTests