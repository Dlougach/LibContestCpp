#include "../NumberTheory/FFT.h"
#include "CppUnitTest.h"

#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace number_theory::fft;


namespace NumberTheoryTests
{

TEST_CLASS(FastFourierTransormTests)
{
public:
	TEST_METHOD(TestSinCos)
	{
		size_t SIZES[] = { 4, 8, 12, 32, 64, 1024, 1 << 12, 1 << 16 };

		for (size_t sz : SIZES) {
			std::vector<double> v1sin(sz), v1cos(sz), v2sin(sz), v2cos(sz);
			fillSinCos(sz, v1sin.data(), v1cos.data());
			fillSinCosNaive(sz, v2sin.data(), v2cos.data());
			for (size_t i = 0; i < sz; ++i) {
				Assert::AreEqual(v1sin[i], v2sin[i], 1e-10);
				Assert::AreEqual(v1cos[i], v2cos[i], 1e-10);
			}
		}
	}
};

}  // namespace NumberTheoryTes