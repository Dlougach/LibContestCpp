#pragma once

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <exception>
#include <memory>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

namespace number_theory {
namespace fft {

#ifdef _MSC_VER
#define aligned_alloc(align, sz) _aligned_malloc(sz, align)
#define aligned_free(ptr) _aligned_free(ptr)
#else
#define aligned_free free
#endif

union alignas(64) CacheLineBlock {
	std::uint8_t uints8[64];
	std::int8_t ints8[64];
	std::uint16_t uints16[32];
	std::int16_t ints16[32];
	std::uint32_t uints32[16];
	std::int32_t ints32[16];
	std::uint64_t uints64[8];
	std::int64_t ints64[8];
	float floats32[16];
	double floats64[8];
	__m256 m256[2];
	__m256d m256d[2];
	__m256i m256i[2];
	__m128 m128[4];
	__m128i m128i[4];
	__m128d m128d[4];

};

static_assert(sizeof(CacheLineBlock) == 64, "CacheLineBlock must have exactly 64 bytes");

namespace detail {

// This is equivalent to reinterpret_cast<T*>(block), but we want to be safe.
template<class T>
T* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	static_assert(sizeof(T) == 0, "Only specializations of ExtractCacheLineBlockPtr can be used");
	std::terminate();
}

template<>
inline std::uint8_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->uints8;
}

template<>
inline std::int8_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->ints8;
}

template<>
inline std::uint16_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->uints16;
}

template<>
inline std::int16_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->ints16;
}

template<>
inline std::uint32_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->uints32;
}

template<>
inline std::int32_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->ints32;
}

template<>
inline std::uint64_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->uints64;
}

template<>
inline std::int64_t* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->ints64;
}

template<>
inline double* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->floats64;
}

template<>
inline float* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->floats32;
}

template<>
inline __m256d* ExtractCacheLineBlockPtr(CacheLineBlock* block) {
	return block->m256d;
}

} // namespace

template<class T>
class AlignedArray {
public:
	AlignedArray() = default;
	AlignedArray(size_t size)
		: size_(size)
	{
		if (size == 0)
			return;
		size_t num_blocks = (size * sizeof(T) + sizeof(CacheLineBlock) - 1) / sizeof(CacheLineBlock);
		blocks_ = static_cast<CacheLineBlock*>(aligned_alloc(alignof(CacheLineBlock), sizeof(CacheLineBlock) * num_blocks));
	}
	~AlignedArray() {
		if (blocks_) {
			aligned_free(blocks_);
		}
	}
	AlignedArray(AlignedArray<T>&& other) {
		std::swap(other.blocks_, blocks_);
		std::swap(other.size_, size_);
	}
	AlignedArray& operator=(AlignedArray<T>&& other) {
		std::swap(other.blocks_, blocks_);
		std::swap(other.size_, size_);
	}

	T* data() {
		return detail::ExtractCacheLineBlockPtr<T>(blocks_);
	}

	const T* data() const {
		return detail::ExtractCacheLineBlockPtr<T>(blocks_);
	}

	size_t size() const {
		return size_;
	}

	T& operator[](size_t idx) {
		return data()[idx];
	}

	const T& operator[](size_t idx) const {
		return data()[idx];
	}
private:
	size_t size_ = 0;
	CacheLineBlock* blocks_ = nullptr;
};

struct ComplexArray {
	ComplexArray(size_t size) :
		real(size),
		imag(size)
	{}
	AlignedArray<__m256d> real, imag;
};

namespace detail {

inline void complexMuld(__m256d left_re, __m256d left_im, __m256d right_re, __m256d right_im, __m256d& out_re, __m256d& out_im) {
	__m256d re_re = _mm256_mul_pd(left_re, right_re);
	__m256d re_im = _mm256_mul_pd(left_re, right_im);
	__m256d im_re = _mm256_mul_pd(left_im, right_re);
	__m256d im_im = _mm256_mul_pd(left_im, right_im);
	out_re = _mm256_sub_pd(re_re, im_im);
	out_im = _mm256_add_pd(re_im, im_re);
}

} // namespace detail

ComplexArray makeSinCos(size_t n) {
	size_t sz = (n + 3) / 4;
	ComplexArray result(sz);
	constexpr int NPRECALC = 4;
	__m256d precalc_sin[NPRECALC], precalc_cos[NPRECALC];
	for (int i = 0; i < NPRECALC; ++i) {
		double vsin[4], vcos[4];
		for (int j = 0; j < 4; ++j) {
			double angle = (i * 4.0 + j) * 2 * std::acos(-1.0) / n;
			vsin[j] = sin(angle);
			vcos[j] = cos(angle);
		}
		precalc_sin[i] = _mm256_load_pd(vsin);
		precalc_cos[i] = _mm256_load_pd(vcos);
	}
	for (size_t i = 0; i < sz; ) {
		double b_angle = i * 4.0 * 2.0 * std::acos(-1.0) / n;
		__m256d bsin = _mm256_set1_pd(sin(b_angle));
		__m256d bcos = _mm256_set1_pd(cos(b_angle));
		for (int j = 0; j < NPRECALC && i < sz; ++j, ++i) {
			detail::complexMuld(bcos, bsin, precalc_cos[j], precalc_sin[j], result.real[i], result.imag[i]);
		}
	}
	return result;
}

// Output should be sufficient for n rounded to next multiple of 4.
void fillSinCos(size_t n, double* out_sin, double* out_cos) {
	ComplexArray arr = makeSinCos(n);
	for (size_t i = 0; i * 4 < n; ++i) {
		_mm256_store_pd(out_sin + i * 4, arr.imag[i]);
		_mm256_store_pd(out_cos + i * 4, arr.real[i]);
	}
}

void fillSinCosNaive(size_t n, double* out_sin, double* out_cos) {
	for (size_t i = 0; i < n; ++i) {
		double angle = i * 2.0 * std::acos(-1.0) / n;
		out_sin[i] = sin(angle);
		out_cos[i] = cos(angle);
	}
}

void fillSinCosNaive2(size_t n, double* out_sin, double* out_cos) {
	constexpr int NPRECALC = 32;
	double precalc_sin[NPRECALC], precalc_cos[NPRECALC];
	for (int i = 0; i < NPRECALC; ++i) {
		double angle = i * 2.0 * std::acos(-1.0) / n;
		precalc_sin[i] = sin(angle);
		precalc_cos[i] = cos(angle);
	}
	for (size_t i = 0; i < n; i += NPRECALC) {
		double angle = i * 2.0 * std::acos(-1.0) / n;
		double bsin = sin(angle);
		double bcos = cos(angle);
		for (int j = 0; j < NPRECALC; ++j) {
			out_sin[i + j] = precalc_sin[j] * bcos + precalc_cos[j] * bsin;
			out_cos[i + j] = precalc_cos[j] * bcos - precalc_sin[j] * bsin;
		}
	}
}

void fillSinCosNaive3(size_t n, double* out_sincos) {
	constexpr int NPRECALC = 64;
	double precalc_sincos[NPRECALC * 2];
	for (int i = 0; i < NPRECALC; ++i) {
		double angle = i * 2.0 * std::acos(-1.0) / n;
		precalc_sincos[2*i] = sin(angle);
		precalc_sincos[2*i + 1] = cos(angle);
	}
	for (size_t i = 0; i < 2 * n; i += NPRECALC * 2) {
		double angle = i * std::acos(-1.0) / n;
		double bsin = sin(angle);
		double bcos = cos(angle);
		for (int j = 0; j < NPRECALC * 2; j += 2) {
			out_sincos[i + j] = precalc_sincos[j] * bcos + precalc_sincos[j + 1] * bsin;
			out_sincos[i + j] = precalc_sincos[j + 1] * bcos - precalc_sincos[j] * bsin;
		}
	}
}


} // namespace fft
} // namespace number_theory

