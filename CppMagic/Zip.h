#pragma once
#include <utility>
#include <tuple>

namespace cpp_magic {
namespace zip {

namespace impl {

template<typename... Iterators>
class zip_iterator {
public:
	using Self = zip_iterator<Iterators...>;
	friend bool operator==(const Self& left, const Self& right) {
		return any_equal_helper<>::call(left.iters_, right.iters_);
	}
	friend bool operator!=(const Self& left, const Self& right) {
		return !(left == right);
	}
	auto operator++() -> decltype(*this) {
		increment_helper<>::call(iters_);
		return (*this);
	}
	auto operator*() -> decltype(std::forward_as_tuple(*std::declval<Iterators>()...)) {
		return dereference_helper<>::call(iters_);
	}
	zip_iterator(const Iterators&... iters)
		: iters_(iters...)
	{}
	std::tuple<Iterators...> iters_;
private:
	template <int c = 0, bool at_end = (c >= sizeof...(Iterators))>
	struct any_equal_helper {
		static bool call(const std::tuple<Iterators...>&, const std::tuple<Iterators...>&) {
			return false;
		}
	};

	template <int c>
	struct any_equal_helper<c, false> {
		static bool call(const std::tuple<Iterators...>& left, const std::tuple<Iterators...>& right) {
			return std::get<c>(left) == std::get<c>(right) || any_equal_helper<c + 1>::call(left, right);
		}
	};

	template <int c = 0, bool at_end = (c >= sizeof...(Iterators))>
	struct increment_helper {
		static void call(std::tuple<Iterators...>&) {
			return;
		}
	};

	template <int c>
	struct increment_helper<c, false> {
		static void call(std::tuple<Iterators...>& param) {
			++std::get<c>(param);
			increment_helper<c + 1>::call(param);
		}
	};

	template <int c = 0, bool at_end = (c >= sizeof...(Iterators))>
	struct dereference_helper {
		static std::tuple<> call(std::tuple<Iterators...>&) {
			return std::forward_as_tuple();
		}
	};

	template <int c>
	struct dereference_helper<c, false> {
		static auto call(std::tuple<Iterators...>& param) {
			return std::tuple_cat(std::forward_as_tuple(*std::get<c>(param)), dereference_helper<c+1>::call(param));
		}
	};
};

template<typename... Containers>
class zip_container_type {
public:
	using iterator = zip_iterator<decltype(std::begin(std::declval<Containers>()))...>;
	zip_container_type(Containers&... containers):
		begin_(iterator(std::begin(containers)...)),
		end_(iterator(std::end(containers)...))
	{}
	iterator begin() const {
		return begin_;
	}
	iterator end() const {
		return end_;
	}
private:
	iterator begin_, end_;
};

}  // namespace impl

template<typename... Containers>
impl::zip_container_type<Containers...> zip(Containers&... containers) {
	return impl::zip_container_type<Containers...>(containers...);
}

}  // namespace zip
}  // namespace cpp_magic