#pragma once

#include <utility>
#include <functional>
#include <iterator>
#include <cassert>
#include <numeric>
#include <vector>
#include <limits>

namespace data_structures {
namespace interval_trees {

// Data should implement the following methods:
//   1) Copy constructor / operator=
//   2) Default constructor
template<class Data, bool is_commutative = false>
class SimpleIntervalTree {
public:
	using Operation = std::function<Data(const Data&, const Data&)>;
	SimpleIntervalTree(size_t size, Operation op, const Data& default_value) {
		op_ = op;
		size_ = size;
		capacity_ = size - 1;
		for (int shift = 1; shift < sizeof(capacity_); shift *= 2) {
			capacity_ |= capacity_ >> shift;
		}
		capacity_ += 1;
		tree_.assign(capacity_ * 2, default_value);
	}

	template<class Iter>
	void FillFrom(Iter begin, Iter end) {
		assert(static_cast<size_t>(std::distance(begin, end)) <= size_);
		Iter it = begin;
		for (size_t offset = capacity_; it != end; ++it, ++offset) {
			tree_[offset] = *it;
		}
		for (size_t offset = capacity_ - 1; offset != 0; --offset) {
			Recompute(offset);
		}
	}

	template<class Container>
	void FillFrom(const Container& cont) {
		FillFrom(std::begin(cont), std::end(cont));
	}

	template<class InitializerListData>
	void FillFrom(std::initializer_list<InitializerListData> initializer_list) {
		FillFrom(std::begin(initializer_list), std::end(initializer_list));
	}

	void SetAt(size_t offset, const Data& data) {
		assert(offset < size_);
		offset += capacity_;
		tree_[offset] = data;
		while (offset != 1) {
			offset /= 2;
			Recompute(offset);
		}
	}

	void AddAt(size_t offset, const Data& data, bool add_front=false) {
		assert(offset < size_);
		offset += capacity_;
		if (is_commutative) {
			// This means we can do everything "easy" way.
			while (offset != 0) {
				ApplyOp(tree_[offset], data, &tree_[offset]);
				OptimizeChooser::Call(tree[offset]);
				offset /= 2;
			}
		}
		else {
			if (add_front) {
				ApplyOp(data, tree_[offset], &tree_[offset]);
			}
			else {
				ApplyOp(tree_[offset], data, &tree_[offset]);
			}
			while (offset != 1) {
				offset /= 2;
				Recompute(offset);
			}
		}
	}

	// left and right - inclusive
	Data RangeReduce(size_t left, size_t right) {
		assert(right < size_);
		assert(left <= right);
		left += capacity_;
		right += capacity_;
		if (left == right) {
			return tree_[left];
		}
		Data left_slope = tree_[left];
		Data right_slope = tree_[right];
		while (right - left != 1) {
			if (left % 2 == 0) {
				ApplyOp(left_slope, tree_[left + 1], &left_slope);
			}
			if (right % 2 == 1) {
				ApplyOp(tree_[right - 1], right_slope, &right_slope);
			}
			left /= 2;
			right /= 2;
		}
		ApplyOp(left_slope, right_slope, &left_slope);
		return left_slope;
	}

private:
	// To make it more flexible.
	void ApplyOp(const Data& left, const Data& right, Data* output) {
		*output = op_(left, right);
	}

	void Recompute(size_t offset) {
		ApplyOp(tree_[offset * 2], tree_[offset * 2 + 1], &tree_[offset]);
	}

	size_t capacity_;
	size_t size_;
	Operation op_;
	std::vector<Data> tree_;
};

template<class T>
struct MinimumIntervalTree : public SimpleIntervalTree<T, true>
{
	using SimpleIntervalTree<T, true>::SimpleIntervalTree;
	MinimumIntervalTree(size_t size) : SimpleIntervalTree<T, true>(
		size,
		[](const T& left, const T& right) {return std::min(left, right); },
		std::numeric_limits<T>::max())
	{}
};

}  // namespace interval_trees
}  // namespace data_structures