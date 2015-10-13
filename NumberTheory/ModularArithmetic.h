#pragma once

#include <cstdint>
#include <utility>
#include <sstream>
#include <string>
#include <cassert>

namespace number_theory {
namespace modular_arithmetic {

template<int MOD>
struct IntegerModulo {
	IntegerModulo() : value_(0) {}
	template<typename T, class=typename std::enable_if<std::is_integral<T>::value>::type>
	IntegerModulo(T value) {
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
	operator T() const {
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
		assert(left.value_ % right.value_ == 0);
		return result * (left.value_ / right.value_);
	}
	IntegerModulo& operator/=(IntegerModulo other) {
		return (*this = *this / other);
	}
private:
	struct unchecked_t_ {};
	IntegerModulo(uint32_t value, unchecked_t_) {
		value_ = value;
	}
	uint32_t value_;
};

template<int MOD>
std::wstring ToString(const IntegerModulo<MOD>& integer_modulo) {
	std::wostringstream oss;
	oss << "(" << static_cast<uint32_t>(integer_modulo) << " mod " << MOD << ")";
	return oss.str();
}

}  // namespace modular_arithmetic
}  // namespace number_theory