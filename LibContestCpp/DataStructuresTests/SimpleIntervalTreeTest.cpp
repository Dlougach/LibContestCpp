#include <algorithm>
#include <numeric>
#include <CppUnitTest.h>
#include "../DataStructures/SimpleIntervalTree.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DataStructuresTests
{		
	TEST_CLASS(IntervalTreeTests)
	{
	public:
		
		TEST_METHOD(MinimumIntervalTreeTest)
		{
			using namespace data_structures::interval_trees;
			MinimumIntervalTree<int> tree(3);
			tree.SetAt(0, -100000);
			Assert::AreEqual(-100000, (int)tree.RangeReduce(0, 1));
			tree.FillFrom({ -1, -1 });
			tree.SetAt(0, 2);
			tree.SetAt(2, 1);
			Assert::AreEqual(2, (int)tree.RangeReduce(0, 0));
			Assert::AreEqual(-1, (int)tree.RangeReduce(0, 1));
			Assert::AreEqual(-1, (int)tree.RangeReduce(0, 2));
			Assert::AreEqual(-1, (int)tree.RangeReduce(1, 1));
			Assert::AreEqual(-1, (int)tree.RangeReduce(1, 2));
			Assert::AreEqual(1, (int)tree.RangeReduce(2, 2));
		}

	};
}