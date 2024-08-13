// C++ utility

#pragma once

#include <algorithm>
#include <functional>

namespace easiglib
{

template<class T, T x, T y>
struct Max {
	static constexpr T val = x > y ? x : y;
};

template<int N>
struct Log2 {
	static constexpr int val = Log2<N / 2>::val + 1;
};

template<>
struct Log2<1> {
	static constexpr int val = 0;
};

constexpr bool is_power_of_2(int v) {
	return v && ((v & (v - 1)) == 0);
}

constexpr int ipow(int a, int b) {
	return b == 0 ? 1 : a * ipow(a, b - 1);
}

class Nocopy {
public:
	Nocopy(const Nocopy &) = delete;
	Nocopy &operator=(const Nocopy &) = delete;

protected:
	constexpr Nocopy() = default;
	~Nocopy() = default;
};

template<class T, class crtpType>
struct crtp {
	T &operator*() {
		return static_cast<T &>(*this);
	}
	T const &operator*() const {
		return static_cast<T const &>(*this);
	}

private:
	crtp() {
	}
	friend crtpType;
};

// Observer pattern
template<typename... DATA>
struct Subject {
	void notify(DATA... args) {
		observer_(args...);
	}
	Subject(std::function<void(DATA...)> observer)
		: observer_(observer) {
	}

private:
	std::function<void(DATA...)> observer_;
};

// N-ary Zip class

// inductive case
template<typename T, typename... Ts>
struct Zip : public Zip<Ts...> {
	static_assert(std::tuple_size<T>::value == std::tuple_size<Zip<Ts...>>::value,
				  "Cannot zip over structures of different sizes");

	using head_value_type = std::tuple<typename T::value_type &>;
	using tail_value_type = typename Zip<Ts...>::value_type;
	using value_type = decltype(std::tuple_cat(std::declval<head_value_type>(), std::declval<tail_value_type>()));

	Zip(T &t, Ts &...ts)
		: Zip<Ts...>(ts...)
		, t_(t) {
	}

	struct iterator : Zip<Ts...>::iterator {
		using head_iterator = typename T::iterator;
		using tail_iterator = typename Zip<Ts...>::iterator;
		bool operator!=(iterator &that) {
			return it != that.it;
		}
		void operator++() {
			++it;
			tail_iterator::operator++();
		}

		value_type operator*() {
			return std::tuple_cat<head_value_type, tail_value_type>(std::forward_as_tuple(*it),
																	tail_iterator::operator*());
		}

		// TODO: pb: copy of i and t?
		iterator(head_iterator i, tail_iterator t)
			: tail_iterator(t)
			, it(i) {
		}

		head_iterator it;
	};

	iterator begin() {
		return iterator(t_.begin(), Zip<Ts...>::begin());
	}
	iterator end() {
		return iterator(t_.end(), Zip<Ts...>::end());
	}
	T &t_;
};


// base case
template<typename T>
struct Zip<T> {
	using value_type = std::tuple<typename T::value_type &>;
	using iterator = typename T::iterator;
	Zip(T &t)
		: t_(t){};
	iterator begin() {
		return t_.begin();
	}
	iterator end() {
		return t_.end();
	}

private:
	T &t_;
};

}

// must implement tuple_size to check size equality
template<typename T, typename... Ts>
struct std::tuple_size<easiglib::Zip<T, Ts...>> {
	static constexpr int value = std::tuple_size<T>::value;
};

namespace easiglib
{

// smart instantiation function
template<typename... Ts>
Zip<Ts...> zip(Ts &...ts) {
	return Zip<Ts...>(ts...);
}

// Overload
template<class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}
