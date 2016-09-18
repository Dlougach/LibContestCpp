#pragma once

#include "Factorization.h"
#include "ModularArithmetic.h"

#include <algorithm>
#include <random>

namespace number_theory {
namespace discreet_logarithm {

template <int modulus>
inline modular_arithmetic::IntegerModulo<modulus> FindPrimitiveRoot(unsigned max_attempts = 1000) {
	using IntMod = modular_arithmetic::IntegerModulo<modulus>;
	if (modulus <= 3) {
		// Special case, because phi is prime.
		return -1;
	}
	int phi = modulus;
	for (auto prime_power : number_theory::factorization::Factorize(modulus)) {
		phi /= prime_power.first;
		phi *= prime_power.first - 1;
	}
	auto phi_decomposition = number_theory::factorization::Factorize(phi);
	// Now let's generate some random numbers from Z_modulus.
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, modulus - 1);
	auto GreatestCommonDivisor = [](int p, int q) {
		while (q != 0) {
			p %= q;
			std::swap(p, q);
		}
		return p;
	};
	for (unsigned i = 0; i < max_attempts; ++i) {
		int candidate;
		do {
			candidate = distribution(generator);
		} while (GreatestCommonDivisor(candidate, modulus) != 1);
		bool is_good = true;
		for (auto prime_power : phi_decomposition) {
			auto pw = IntMod(candidate).ToPower(phi / prime_power.first);
			if (pw == 1) {
				is_good = false;
				break;
			}
		}
		if (is_good) {
			return candidate;
		}
	}
	return 0; // 0 will be the sign of failure.
}

} // namespace discreet_logarithm
} // namespace number_thery