#pragma once

#include <cassert>
#include <cstdint>
#include <utility>
#include <sstream>
#include <string>
#include <stdexcept>
#include <limits>

namespace number_theory {
namespace modular_arithmetic {

class division_impossible_error : public std::domain_error {
public:
	division_impossible_error() : domain_error("Division impossible for given operands") {}
};

template<int modulus>
struct IntegerModulo {
	static constexpr int MOD = modulus;
	IntegerModulo() : value_(0) {}
	template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	IntegerModulo(T value) {
		static_assert(MOD > 0, "modulus can't be <= 0");
		if (value >= MOD || value < 0) {
			value_ = static_cast<uint32_t>(value % MOD);
			if (value_ >= MOD) {
				// integer overflow
				value_ += MOD;
			}
		}
		else {
			value_ = static_cast<uint32_t>(value);
		}
	}
	IntegerModulo(const IntegerModulo&) = default;
	IntegerModulo(IntegerModulo&&) = default;
	IntegerModulo& operator=(const IntegerModulo&) = default;
	IntegerModulo& operator=(IntegerModulo&&) = default;
	bool operator==(const IntegerModulo& other) const {
		return value_ == other.value_;
	}

	template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	explicit operator T() const {
		return static_cast<T>(value_);
	}

	friend IntegerModulo operator+(const IntegerModulo& left, const IntegerModulo& right) {
		uint32_t result = left.value_ + right.value_;
		if (result >= MOD) {
			result -= MOD;
		}
		return IntegerModulo(result, unchecked_t_());
	}
	IntegerModulo& operator+=(const IntegerModulo& other) {
		return (*this = *this + other);
	}
	friend IntegerModulo operator-(const IntegerModulo& left, const IntegerModulo& right) {
		uint32_t result = left.value_;
		if (result < right.value_)
			result += MOD;
		result -= right.value_;
		return IntegerModulo(result, unchecked_t_());
	}
	IntegerModulo& operator-=(const IntegerModulo& other) {
		return (*this = *this + other);
	}
	friend IntegerModulo operator*(const IntegerModulo& left, const IntegerModulo& right) {
		return static_cast<uint64_t>(left) * static_cast<uint64_t>(right);
	}
	IntegerModulo& operator*=(const IntegerModulo& other) {
		return (*this = *this * other);
	}
	friend IntegerModulo operator/(const IntegerModulo& left, IntegerModulo right) {
		IntegerModulo result = 1;
		assert(right.value_ != 0);
		while (MOD % right.value_ != 0) {
			uint32_t cnt = MOD / right.value_ + 1;
			result *= cnt;
			right.value_ *= cnt;
			right.value_ -= MOD;
		}
		if (left.value_ % right.value_ != 0)
			throw division_impossible_error();
		return result * (left.value_ / right.value_);
	}
	IntegerModulo& operator/=(IntegerModulo other) {
		return (*this = *this / other);
	}
	IntegerModulo ToPower(unsigned long long power) const {
		IntegerModulo curPow = *this;
		IntegerModulo result = 1;
		while (power != 0) {
			if (power % 2 == 1) {
				result *= curPow;
			}
			curPow *= curPow;
			power /= 2;
		}
		return result;
	}
private:
	struct unchecked_t_ {};
	IntegerModulo(uint32_t value, unchecked_t_) {
		value_ = value;
	}
	uint32_t value_;
};

template<class Iter1, class Iter2>
typename std::enable_if<
	std::is_same<typename std::iterator_traits<Iter1>::value_type, typename std::iterator_traits<Iter2>::value_type>::value,
	typename std::iterator_traits<Iter1>::value_type
>::type FastDotProduct(
	Iter1 begin1,
	Iter2 begin2,
	size_t n) {
	static constexpr int kMod = std::iterator_traits<Iter1>::value_type::MOD;
	static constexpr uint64_t kModMax = (std::numeric_limits<uint64_t>::max() / kMod / 2) * kMod;
	uint64_t result = 0;
	while (n--) {
		result += static_cast<uint64_t>(*begin1) * static_cast<uint64_t>(*begin2);
		if (result >= kModMax) {
			result -= kModMax;
		}
		++begin1;
		++begin2;
	}
	return result; // Implicitly casted.
}

template<int MOD>
std::wstring ToString(const IntegerModulo<MOD>& integer_modulo) {
	std::wostringstream oss;
	oss << "(" << static_cast<uint32_t>(integer_modulo) << " mod " << MOD << ")";
	return oss.str();
}

}  // namespace modular_arithmetic
}  // namespace number_theory