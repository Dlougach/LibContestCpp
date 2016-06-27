#include <vector>
#include <tuple>
#include "CppUnitTest.h"
#include "../CppMagic/Zip.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CppMagicTests
{
TEST_CLASS(ZipTest)
{
public:

	TEST_METHOD(TestSomethingCompiles)
	{
		using namespace cpp_magic::zip;
		const int ar1[] = { 1, 2, 3, 4, 5 };
		const int ar2[] = { 5, 4, 3, 2 };
		auto z = zip(ar1, ar2);
		Assert::IsFalse(z.begin() == z.end());
		Assert::IsTrue(z.begin() == z.begin());
	}

	TEST_METHOD(TestZip) {
		using namespace cpp_magic::zip;
		const int ar1[] = { 1, 2, 3, 4, 5 };
		const int ar2[] = { 5, 4, 3, 2 };
		std::vector<std::tuple<int, int>> actual, expected;
		for (auto tup : zip(ar1, ar2)) {
			actual.push_back(tup);
		}
		expected = { {1, 5}, {2, 4}, {3, 3}, {4, 2} };
		Assert::AreEqual(expected.size(), actual.size());
		for (auto tup : zip(expected, actual)) {
			Assert::IsTrue(std::get<0>(tup) == std::get<1>(tup));
		}
	}
};
}