/* Copyright 2018-2019 Dimitrij Mijoski, Sander van Geloven
 *
 * This file is part of Nuspell.
 *
 * Nuspell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nuspell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Nuspell.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Data structures, private header.
 */

#ifndef NUSPELL_STRUCTURES_HXX
#define NUSPELL_STRUCTURES_HXX

#include "string_utils.hxx"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/container/small_vector.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace nuspell {

/**
 * @brief A Set class backed by a string. Very useful for small sets.
 *
 * Has the same interface as std::set.
 */
template <class CharT>
class String_Set {
      private:
	std::basic_string<CharT> d;
	auto sort_uniq() -> void
	{
		auto first = begin();
		auto last = end();
		using t = traits_type;
		sort(first, last, t::lt);
		d.erase(unique(first, last, t::eq), last);
	}
	struct Char_Traits_Less_Than {
		auto operator()(CharT a, CharT b)
		{
			return traits_type::lt(a, b);
		}
	};

      public:
	using StrT = std::basic_string<CharT>;
	using traits_type = typename StrT::traits_type;

	using key_type = typename StrT::value_type;
	using key_compare = Char_Traits_Less_Than;
	using value_type = typename StrT::value_type;
	using value_compare = key_compare;
	using allocator_type = typename StrT::allocator_type;
	using pointer = typename StrT::pointer;
	using const_pointer = typename StrT::const_pointer;
	using reference = typename StrT::reference;
	using const_reference = typename StrT::const_reference;
	using size_type = typename StrT::size_type;
	using difference_type = typename StrT::difference_type;
	using iterator = typename StrT::iterator;
	using const_iterator = typename StrT::const_iterator;
	using reverse_iterator = typename StrT::reverse_iterator;
	using const_reverse_iterator = typename StrT::const_reverse_iterator;

	String_Set() = default;
	String_Set(const StrT& s) : d(s) { sort_uniq(); }
	String_Set(StrT&& s) : d(move(s)) { sort_uniq(); }
	String_Set(const CharT* s) : d(s) { sort_uniq(); }
	template <class InputIterator>
	String_Set(InputIterator first, InputIterator last) : d(first, last)
	{
		sort_uniq();
	}
	String_Set(std::initializer_list<value_type> il) : d(il)
	{
		sort_uniq();
	}

	auto& operator=(const StrT& s)
	{
		d = s;
		sort_uniq();
		return *this;
	}
	auto& operator=(StrT&& s)
	{
		d = move(s);
		sort_uniq();
		return *this;
	}
	auto& operator=(std::initializer_list<value_type> il)
	{
		d = il;
		sort_uniq();
		return *this;
	}
	auto& operator=(const CharT* s)
	{
		d = s;
		sort_uniq();
		return *this;
	}

	// non standard underlying storage access:
	auto& data() const { return d; }
	operator const StrT&() const noexcept { return d; }

	// iterators:
	iterator begin() noexcept { return d.begin(); }
	const_iterator begin() const noexcept { return d.begin(); }
	iterator end() noexcept { return d.end(); }
	const_iterator end() const noexcept { return d.end(); }

	reverse_iterator rbegin() noexcept { return d.rbegin(); }
	const_reverse_iterator rbegin() const noexcept { return d.rbegin(); }
	reverse_iterator rend() noexcept { return d.rend(); }
	const_reverse_iterator rend() const noexcept { return d.rend(); }

	const_iterator cbegin() const noexcept { return d.cbegin(); }
	const_iterator cend() const noexcept { return d.cend(); }
	const_reverse_iterator crbegin() const noexcept { return d.crbegin(); }
	const_reverse_iterator crend() const noexcept { return d.crend(); }

	// capacity:
	bool empty() const noexcept { return d.empty(); }
	size_type size() const noexcept { return d.size(); }
	size_type max_size() const noexcept { return d.max_size(); }

	// modifiers:
	std::pair<iterator, bool> insert(const value_type& x)
	{
		auto it = lower_bound(x);
		if (it != end() && *it == x) {
			return {it, false};
		}
		auto ret = d.insert(it, x);
		return {ret, true};
	}
	// std::pair<iterator, bool> insert(value_type&& x);
	iterator insert(iterator hint, const value_type& x)
	{
		if (hint == end() || traits_type::lt(x, *hint)) {
			if (hint == begin() ||
			    traits_type::lt(*(hint - 1), x)) {
				return d.insert(hint, x);
			}
		}
		return insert(x).first;
	}

	// iterator insert(const_iterator hint, value_type&& x);
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		d.insert(end(), first, last);
		sort_uniq();
	}
	void insert(std::initializer_list<value_type> il)
	{
		d.insert(end(), il);
		sort_uniq();
	}
	template <class... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		return insert(CharT(args...));
	}

	template <class... Args>
	iterator emplace_hint(iterator hint, Args&&... args)
	{
		return insert(hint, CharT(args...));
	}

	iterator erase(iterator position) { return d.erase(position); }
	size_type erase(const key_type& x)
	{
		auto i = d.find(x);
		if (i != d.npos) {
			d.erase(i, 1);
			return true;
		}
		return false;
	}
	iterator erase(iterator first, iterator last)
	{
		return d.erase(first, last);
	}
	void swap(String_Set& s) { d.swap(s.d); }
	void clear() noexcept { d.clear(); }

	// non standrd modifiers:
	auto insert(const StrT& s) -> void
	{
		d += s;
		sort_uniq();
	}
	auto operator+=(const StrT& s) -> String_Set
	{
		insert(s);
		return *this;
	}

	// observers:
	key_compare key_comp() const { return Char_Traits_Less_Than(); }
	value_compare value_comp() const { return key_comp(); }

	// set operations:
      private:
	auto lookup(const key_type& x) const
	{
		auto i = d.find(x);
		if (i != d.npos)
			i = d.size();
		return i;
	}

      public:
	iterator find(const key_type& x) { return begin() + lookup(x); }
	const_iterator find(const key_type& x) const
	{
		return begin() + lookup(x);
	}
	size_type count(const key_type& x) const { return d.find(x) != d.npos; }

	iterator lower_bound(const key_type& x)
	{
		return std::lower_bound(begin(), end(), x, traits_type::lt);
	}

	const_iterator lower_bound(const key_type& x) const
	{
		return std::lower_bound(begin(), end(), x, traits_type::lt);
	}
	iterator upper_bound(const key_type& x)
	{
		return std::upper_bound(begin(), end(), x, traits_type::lt);
	}

	const_iterator upper_bound(const key_type& x) const
	{
		return std::upper_bound(begin(), end(), x, traits_type::lt);
	}
	std::pair<iterator, iterator> equal_range(const key_type& x)
	{
		return std::equal_range(begin(), end(), x, traits_type::lt);
	}

	std::pair<const_iterator, const_iterator>
	equal_range(const key_type& x) const
	{
		return std::equal_range(begin(), end(), x, traits_type::lt);
	}

	// non standard set operations:
	bool contains(const key_type& x) const { return count(x); }

	// compare
	bool operator<(const String_Set& rhs) const { return d < rhs.d; }
	bool operator<=(const String_Set& rhs) const { return d <= rhs.d; }
	bool operator==(const String_Set& rhs) const { return d == rhs.d; }
	bool operator!=(const String_Set& rhs) const { return d != rhs.d; }
	bool operator>=(const String_Set& rhs) const { return d >= rhs.d; }
	bool operator>(const String_Set& rhs) const { return d > rhs.d; }
};

template <class CharT>
auto swap(String_Set<CharT>& a, String_Set<CharT>& b)
{
	a.swap(b);
}

using Flag_Set = String_Set<char16_t>;

template <class CharT>
class Substr_Replacer {
      public:
	using StrT = std::basic_string<CharT>;
	using StrViewT = my_string_view<CharT>;
	using Pair_StrT = std::pair<StrT, StrT>;
	using Table_Pairs = std::vector<Pair_StrT>;

      private:
	Table_Pairs table;
	auto sort_uniq() -> void; // implemented in cxx
	auto find_match(my_string_view<CharT> s) const;

      public:
	Substr_Replacer() = default;
	Substr_Replacer(const Table_Pairs& v) : table(v) { sort_uniq(); }
	Substr_Replacer(const Table_Pairs&& v) : table(move(v)) { sort_uniq(); }

	auto& operator=(const Table_Pairs& v)
	{
		table = v;
		sort_uniq();
		return *this;
	}
	auto& operator=(const Table_Pairs&& v)
	{
		table = move(v);
		sort_uniq();
		return *this;
	}

	template <class Range>
	auto& operator=(const Range& range)
	{
		table.assign(std::begin(range), std::end(range));
		sort_uniq();
		return *this;
	}

	auto replace(StrT& s) const -> StrT&; // implemented in cxx
	auto replace_copy(StrT s) const -> StrT
	{
		replace(s);
		return s;
	}
};
template <class CharT>
auto Substr_Replacer<CharT>::sort_uniq() -> void
{
	auto first = begin(table);
	auto last = end(table);
	sort(first, last, [](auto& a, auto& b) { return a.first < b.first; });
	auto it = unique(first, last,
	                 [](auto& a, auto& b) { return a.first == b.first; });
	table.erase(it, last);

	// remove empty key ""
	if (!table.empty() && table.front().first.empty())
		table.erase(begin(table));
}

template <class CharT>
auto Substr_Replacer<CharT>::find_match(my_string_view<CharT> s) const
{
	auto& t = table;
	struct Comparer_Str_Rep {
		auto static cmp_prefix_of(const StrT& p, StrViewT of)
		{
			return p.compare(0, p.npos, of.data(),
			                 std::min(p.size(), of.size()));
		}
		auto operator()(const Pair_StrT& a, StrViewT b)
		{
			return cmp_prefix_of(a.first, b) < 0;
		}
		auto operator()(StrViewT a, const Pair_StrT& b)
		{
			return cmp_prefix_of(b.first, a) > 0;
		}
		auto static eq(const Pair_StrT& a, StrViewT b)
		{
			return cmp_prefix_of(a.first, b) == 0;
		}
	};
	Comparer_Str_Rep csr;
	auto it = begin(t);
	auto last_match = end(t);
	for (;;) {
		auto it2 = upper_bound(it, end(t), s, csr);
		if (it2 == it) {
			// not found, s is smaller that the range
			break;
		}
		--it2;
		if (csr.eq(*it2, s)) {
			// Match found. Try another search maybe for
			// longer.
			last_match = it2;
			it = ++it2;
		}
		else {
			// not found, s is greater that the range
			break;
		}
	}
	return last_match;
}

template <class CharT>
auto Substr_Replacer<CharT>::replace(StrT& s) const -> StrT&
{

	if (table.empty())
		return s;
	for (size_t i = 0; i < s.size(); /*no increment here*/) {
		auto substr = my_string_view<CharT>(&s[i], s.size() - i);
		auto it = find_match(substr);
		if (it != end(table)) {
			auto& match = *it;
			// match found. match.first is the found string,
			// match.second is the replacement.
			s.replace(i, match.first.size(), match.second);
			i += match.second.size();
			continue;
		}
		++i;
	}
	return s;
}

template <class CharT>
class Break_Table {
      public:
	using StrT = std::basic_string<CharT>;
	using Table_Str = std::vector<StrT>;
	using iterator = typename Table_Str::iterator;
	using const_iterator = typename Table_Str::const_iterator;

      private:
	Table_Str table;
	size_t start_word_breaks_last_idx = 0;
	size_t end_word_breaks_last_idx = 0;

	auto order_entries() -> void; // implemented in cxx

      public:
	Break_Table() = default;
	Break_Table(const Table_Str& v) : table(v) { order_entries(); }
	Break_Table(Table_Str&& v) : table(move(v)) { order_entries(); }

	auto& operator=(const Table_Str& v)
	{
		table = v;
		order_entries();
		return *this;
	}

	auto& operator=(Table_Str&& v)
	{
		table = move(v);
		order_entries();
		return *this;
	}

	template <class Range>
	auto& operator=(const Range& range)
	{
		table.assign(std::begin(range), std::end(range));
		order_entries();
		return *this;
	}

	auto start_word_breaks() const -> boost::iterator_range<const_iterator>
	{
		return {begin(table),
		        begin(table) + start_word_breaks_last_idx};
	}
	auto end_word_breaks() const -> boost::iterator_range<const_iterator>
	{
		return {begin(table) + start_word_breaks_last_idx,
		        begin(table) + end_word_breaks_last_idx};
	}
	auto middle_word_breaks() const -> boost::iterator_range<const_iterator>
	{
		return {begin(table) + end_word_breaks_last_idx, end(table)};
	}
};
template <class CharT>
auto Break_Table<CharT>::order_entries() -> void
{
	auto it = remove_if(begin(table), end(table), [](auto& s) {
		return s.empty() ||
		       (s.size() == 1 && (s[0] == '^' || s[0] == '$'));
	});
	table.erase(it, end(table));

	auto is_start_word_break = [=](auto& x) { return x[0] == '^'; };
	auto is_end_word_break = [=](auto& x) { return x.back() == '$'; };
	auto start_word_breaks_last =
	    partition(begin(table), end(table), is_start_word_break);
	start_word_breaks_last_idx = start_word_breaks_last - begin(table);

	for_each(begin(table), start_word_breaks_last,
	         [](auto& e) { e.erase(0, 1); });

	auto end_word_breaks_last =
	    partition(start_word_breaks_last, end(table), is_end_word_break);
	end_word_breaks_last_idx = end_word_breaks_last - begin(table);

	for_each(start_word_breaks_last, end_word_breaks_last,
	         [](auto& e) { e.pop_back(); });
}

struct identity {
	template <class T>
	constexpr auto operator()(T&& t) const noexcept -> T&&
	{
		return std::forward<T>(t);
	}
};

template <class Value, class Key = Value, class KeyExtract = identity,
          class Hash = std::hash<Key>, class KeyEqual = std::equal_to<>>
class Hash_Multiset {
      private:
	using bucket_type = boost::container::small_vector<Value, 1>;
	static constexpr float max_load_fact = 7.0 / 8.0;
	std::vector<bucket_type> data;
	size_t sz = 0;
	size_t max_load_factor_capacity = 0;
	KeyExtract key_extract = {};
	Hash hash = {};
	KeyEqual equal = {};

      public:
	using key_type = Key;
	using value_type = Value;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using hasher = Hash;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename bucket_type::pointer;
	using const_pointer = typename bucket_type::const_pointer;
	using local_iterator = typename bucket_type::iterator;
	using local_const_iterator = typename bucket_type::const_iterator;

	Hash_Multiset() : data(16) {}

	auto size() const { return sz; }
	auto empty() const { return size() == 0; }

	auto rehash(size_t count)
	{
		if (empty()) {
			size_t capacity = 16;
			while (capacity <= count)
				capacity <<= 1;
			data.resize(capacity);
			max_load_factor_capacity =
			    std::ceil(capacity * max_load_fact);
			return;
		}
		if (count < size() / max_load_fact)
			count = size() / max_load_fact;
		auto n = Hash_Multiset();
		n.rehash(count);
		for (auto& b : data) {
			for (auto& x : b) {
				n.insert(x);
			}
		}
		data.swap(n.data);
		sz = n.sz;
		max_load_factor_capacity = n.max_load_factor_capacity;
	}

	auto reserve(size_t count) -> void
	{
		rehash(std::ceil(count / max_load_fact));
	}

	auto insert(const_reference value)
	{
		using namespace std;
		if (sz == max_load_factor_capacity) {
			reserve(sz + 1);
		}
		auto&& key = key_extract(value);
		auto h = hash(key);
		auto h_mod = h & (data.size() - 1);
		auto& bucket = data[h_mod];
		if (bucket.size() == 0 || bucket.size() == 1 ||
		    equal(key, key_extract(bucket.back()))) {
			bucket.push_back(value);
			++sz;
			return end(bucket) - 1;
		}
		auto last =
		    std::find_if(rbegin(bucket), rend(bucket), [&](auto& x) {
			    return equal(key, key_extract(x));
		    });
		if (last != rend(bucket)) {
			auto ret = bucket.insert(last.base(), value);
			++sz;
			return ret;
		}

		bucket.push_back(value);
		++sz;
		return end(bucket) - 1;
	}
	template <class... Args>
	auto emplace(Args&&... a)
	{
		return insert(value_type(std::forward<Args>(a)...));
	}

	// Note, leaks non-const iterator. do not modify
	// the key part of the returned value(s).
	auto equal_range_nonconst_unsafe(const key_type& key)
	    -> std::pair<local_iterator, local_iterator>
	{
		using namespace std;
		if (data.empty())
			return {};
		auto h = hash(key);
		auto h_mod = h & (data.size() - 1);
		auto& bucket = data[h_mod];
		if (bucket.empty())
			return {};
		if (bucket.size() == 1) {
			if (equal(key, key_extract(bucket.front())))
				return {begin(bucket), end(bucket)};
			return {};
		}
		auto first =
		    std::find_if(begin(bucket), end(bucket), [&](auto& x) {
			    return equal(key, key_extract(x));
		    });
		if (first == end(bucket))
			return {};
		auto next = first + 1;
		if (next == end(bucket) || !equal(key, key_extract(*next)))
			return {first, next};
		auto last =
		    std::find_if(rbegin(bucket), rend(bucket), [&](auto& x) {
			    return equal(key, key_extract(x));
		    });
		return {first, last.base()};
	}

	auto equal_range(const key_type& key) const
	    -> std::pair<local_const_iterator, local_const_iterator>
	{
		using namespace std;
		if (data.empty())
			return {};
		auto h = hash(key);
		auto h_mod = h & (data.size() - 1);
		auto& bucket = data[h_mod];
		if (bucket.empty())
			return {};
		if (bucket.size() == 1) {
			if (equal(key, key_extract(bucket.front())))
				return {begin(bucket), end(bucket)};
			return {};
		}
		auto first =
		    std::find_if(begin(bucket), end(bucket), [&](auto& x) {
			    return equal(key, key_extract(x));
		    });
		if (first == end(bucket))
			return {};
		auto next = first + 1;
		if (next == end(bucket) || !equal(key, key_extract(*next)))
			return {first, next};
		auto last =
		    std::find_if(rbegin(bucket), rend(bucket), [&](auto& x) {
			    return equal(key, key_extract(x));
		    });
		return {first, last.base()};
	}
};

/**
 * @brief Limited regular expression matching used in affix entries.
 *
 * This implementation increases performance over the regex implementation in
 * the standard library.
 */
template <class CharT>
class Condition {
      public:
	enum Span_Type {
		NORMAL /**< normal character */,
		DOT /**< wildcard character */,
		ANY_OF /**< set of possible characters */,
		NONE_OF /**< set of excluding characters */
	};
	using StrT = std::basic_string<CharT>;

      private:
	StrT cond;
	std::vector<std::tuple<size_t, size_t, Span_Type>>
	    spans; // pos, len, type
	size_t length = 0;

	auto construct() -> void; // implemented in cxx

      public:
	Condition() = default;
	Condition(const StrT& condition) : cond(condition) { construct(); }
	Condition(StrT&& condition) : cond(move(condition)) { construct(); }
	auto match(const StrT& s, size_t pos = 0, size_t len = StrT::npos) const
	    -> bool; // implemented in cxx
	auto match_prefix(const StrT& s) const { return match(s, 0, length); }
	auto match_suffix(const StrT& s) const
	{
		if (length > s.size())
			return false;
		return match(s, s.size() - length, length);
	}
};
template <class CharT>
auto Condition<CharT>::construct() -> void
{
	size_t i = 0;
	for (; i != cond.size();) {
		size_t j = cond.find_first_of(NUSPELL_LITERAL(CharT, "[]."), i);
		if (i != j) {
			if (j == cond.npos) {
				spans.emplace_back(i, cond.size() - i, NORMAL);
				length += cond.size() - i;
				break;
			}
			spans.emplace_back(i, j - i, NORMAL);
			length += j - i;
			i = j;
		}
		if (cond[i] == '.') {
			spans.emplace_back(i, 1, DOT);
			++length;
			++i;
			continue;
		}
		if (cond[i] == ']') {
			auto what =
			    "closing bracket has no matching opening bracket";
			throw std::invalid_argument(what);
		}
		if (cond[i] == '[') {
			++i;
			if (i == cond.size()) {
				auto what = "opening bracket has no matching "
				            "closing bracket";
				throw std::invalid_argument(what);
			}
			Span_Type type;
			if (cond[i] == '^') {
				type = NONE_OF;
				++i;
			}
			else {
				type = ANY_OF;
			}
			j = cond.find(']', i);
			if (j == i) {
				auto what = "empty bracket expression";
				throw std::invalid_argument(what);
			}
			if (j == cond.npos) {
				auto what = "opening bracket has no matching "
				            "closing bracket";
				throw std::invalid_argument(what);
			}
			spans.emplace_back(i, j - i, type);
			++length;
			i = j + 1;
		}
	}
}

/**
 * Checks if provided string matched the condition.
 *
 * @param s string to check if it matches the condition.
 * @param pos start position for string, default is 0.
 * @param len length of string counting from the start position.
 * @return The valueof true when string matched condition.
 */
template <class CharT>
auto Condition<CharT>::match(const StrT& s, size_t pos, size_t len) const
    -> bool
{
	if (pos > s.size()) {
		throw std::out_of_range(
		    "position on the string is out of bounds");
	}
	if (s.size() - pos < len)
		len = s.size() - pos;
	if (len != length)
		return false;

	size_t i = pos;
	for (auto& x : spans) {
		auto x_pos = std::get<0>(x);
		auto x_len = std::get<1>(x);
		auto x_type = std::get<2>(x);

		using tr = typename StrT::traits_type;
		switch (x_type) {
		case NORMAL:
			if (tr::compare(&s[i], &cond[x_pos], x_len) == 0)
				i += x_len;
			else
				return false;
			break;
		case DOT:
			++i;
			break;
		case ANY_OF:
			if (tr::find(&cond[x_pos], x_len, s[i]))
				++i;
			else
				return false;
			break;
		case NONE_OF:
			if (tr::find(&cond[x_pos], x_len, s[i]))
				return false;
			else
				++i;
			break;
		}
	}
	return true;
}

template <class CharT>
class Prefix {
      public:
	using StrT = std::basic_string<CharT>;
	using CondT = Condition<CharT>;
	using value_type = CharT;

	char16_t flag = 0;
	bool cross_product = false;
	StrT stripping;
	StrT appending;
	Flag_Set cont_flags;
	CondT condition;

	Prefix() = default;
	Prefix(char16_t flag, bool cross_product, const StrT& strip,
	       const StrT& append, const std::u16string& cont_flags,
	       const StrT& condition)
	    : flag(flag), cross_product(cross_product), stripping(strip),
	      appending(append), cont_flags(cont_flags), condition(condition)
	{
	}

	auto to_root(StrT& word) const -> StrT&
	{
		return word.replace(0, appending.size(), stripping);
	}
	auto to_root_copy(StrT word) const -> StrT
	{
		to_root(word);
		return word;
	}

	auto to_derived(StrT& word) const -> StrT&
	{
		return word.replace(0, stripping.size(), appending);
	}
	auto to_derived_copy(StrT word) const -> StrT
	{
		to_derived(word);
		return word;
	}

	auto check_condition(const StrT& word) const -> bool
	{
		return condition.match_prefix(word);
	}
};

template <class CharT>
class Suffix {
      public:
	using StrT = std::basic_string<CharT>;
	using CondT = Condition<CharT>;
	using value_type = CharT;

	char16_t flag = 0;
	bool cross_product = false;
	StrT stripping;
	StrT appending;
	Flag_Set cont_flags;
	CondT condition;

	Suffix() = default;
	Suffix(char16_t flag, bool cross_product, const StrT& strip,
	       const StrT& append, const std::u16string& cont_flags,
	       const StrT& condition)
	    : flag(flag), cross_product(cross_product), stripping(strip),
	      appending(append), cont_flags(cont_flags), condition(condition)
	{
	}

	auto to_root(StrT& word) const -> StrT&
	{
		return word.replace(word.size() - appending.size(),
		                    appending.size(), stripping);
	}
	auto to_root_copy(StrT word) const -> StrT
	{
		to_root(word);
		return word;
	}

	auto to_derived(StrT& word) const -> StrT&
	{
		return word.replace(word.size() - stripping.size(),
		                    stripping.size(), appending);
	}
	auto to_derived_copy(StrT word) const -> StrT
	{
		to_derived(word);
		return word;
	}

	auto check_condition(const StrT& word) const -> bool
	{
		return condition.match_suffix(word);
	}
};

using boost::multi_index::member;

template <class CharT, class AffixT>
using Affix_Table_Base =
    Hash_Multiset<AffixT, my_string_view<CharT>,
                  member<AffixT, std::basic_string<CharT>, &AffixT::appending>>;

template <class CharT, class AffixT>
class Affix_Table : private Affix_Table_Base<CharT, AffixT> {
      private:
	Flag_Set all_cont_flags;

      public:
	using base = Affix_Table_Base<CharT, AffixT>;
	using iterator = typename base::local_const_iterator;
	template <class... Args>
	auto emplace(Args&&... a)
	{
		auto it = base::emplace(std::forward<Args>(a)...);
		all_cont_flags += it->cont_flags;
		return it;
	}
	auto equal_range(my_string_view<CharT> appending) const
	{
		return base::equal_range(appending);
	}
	auto has_continuation_flags() const
	{
		return all_cont_flags.size() != 0;
	}
	auto has_continuation_flag(char16_t flag) const
	{
		return all_cont_flags.contains(flag);
	}
};

template <class CharT>
using Prefix_Table = Affix_Table<CharT, Prefix<CharT>>;

template <class CharT>
using Suffix_Table = Affix_Table<CharT, Suffix<CharT>>;

template <class CharT>
class String_Pair {
	using StrT = std::basic_string<CharT>;
	size_t i = 0;
	StrT s;

      public:
	String_Pair() = default;
	template <class Str1>
	/**
	 * @brief Construct string pair
	 *
	 * Constructs a string pair from a single string containing a pair of
	 * strings and an index where the split resides.
	 *
	 * @param str the string that can be split into a pair.
	 * @param i the index where the string is split.
	 * @throws std::out_of_range
	 */
	String_Pair(Str1&& str, size_t i) : i(i), s(std::forward<Str1>(str))
	{
		if (i > s.size()) {
			throw std::out_of_range("word split is too long");
		}
	}

	template <
	    class Str1, class Str2,
	    class = std::enable_if_t<
	        std::is_same<std::remove_reference_t<Str1>, StrT>::value &&
	        std::is_same<std::remove_reference_t<Str2>, StrT>::value>>
	String_Pair(Str1&& first, Str2&& second)
	    : i(first.size()) /* must be init before s, before we move
	                         from first */
	      ,
	      s(std::forward<Str1>(first) + std::forward<Str2>(second))

	{
	}
	auto first() const { return my_string_view<CharT>(s).substr(0, i); }
	auto second() const { return my_string_view<CharT>(s).substr(i); }
	auto first(my_string_view<CharT> x)
	{
		s.replace(0, i, x.data(), x.size());
		i = x.size();
	}
	auto second(my_string_view<CharT> x)
	{
		s.replace(i, s.npos, x.data(), x.size());
	}
	auto& str() const { return s; }
	auto idx() const { return i; }
};
template <class CharT>
struct Compound_Pattern {
	using StrT = std::basic_string<CharT>;

	String_Pair<CharT> begin_end_chars;
	StrT replacement;
	char16_t first_word_flag = 0;
	char16_t second_word_flag = 0;
	bool match_first_only_unaffixed_or_zero_affixed = false;
};

class Compound_Rule_Table {
	std::vector<std::u16string> rules;
	Flag_Set all_flags;

	auto fill_all_flags() -> void;

      public:
	Compound_Rule_Table() = default;
	Compound_Rule_Table(const std::vector<std::u16string>& tbl) : rules(tbl)
	{
		fill_all_flags();
	}
	Compound_Rule_Table(std::vector<std::u16string>&& tbl)
	    : rules(move(tbl))
	{
		fill_all_flags();
	}
	auto operator=(const std::vector<std::u16string>& tbl)
	{
		rules = tbl;
		fill_all_flags();
		return *this;
	}
	auto operator=(std::vector<std::u16string>&& tbl)
	{
		rules = move(tbl);
		fill_all_flags();
		return *this;
	}
	auto empty() const { return rules.empty(); }
	auto has_any_of_flags(const Flag_Set& f) const -> bool;
	auto match_any_rule(const std::vector<const Flag_Set*> data) const
	    -> bool;
};
auto inline Compound_Rule_Table::fill_all_flags() -> void
{
	for (auto& f : rules) {
		all_flags += f;
	}
	all_flags.erase(u'?');
	all_flags.erase(u'*');
}

auto inline Compound_Rule_Table::has_any_of_flags(const Flag_Set& f) const
    -> bool
{
	using std::begin;
	using std::end;
	struct Out_Iter_One_Bool {
		bool* value = nullptr;
		auto& operator++() { return *this; }
		auto& operator++(int) { return *this; }
		auto& operator*() { return *this; }
		auto& operator=(char16_t)
		{
			*value = true;
			return *this;
		}
	};
	auto has_intersection = false;
	auto out_it = Out_Iter_One_Bool{&has_intersection};
	std::set_intersection(begin(all_flags), end(all_flags), begin(f),
	                      end(f), out_it);
	return has_intersection;
}

auto inline match_compund_rule(const std::vector<const Flag_Set*>& words_data,
                               const std::u16string& pattern)
{
	return match_simple_regex(
	    words_data, pattern,
	    [](const Flag_Set* d, char16_t p) { return d->contains(p); });
}

auto inline Compound_Rule_Table::match_any_rule(
    const std::vector<const Flag_Set*> data) const -> bool
{
	return any_of(begin(rules), end(rules), [&](const std::u16string& p) {
		return match_compund_rule(data, p);
	});
}

/**
 * @brief Vector of strings that recycles erased strings
 */
template <class CharT>
class List_Basic_Strings {
	using VecT = std::vector<std::basic_string<CharT>>;
	VecT d;
	size_t sz = 0;

      public:
	using value_type = typename VecT::value_type;
	using allocator_type = typename VecT::allocator_type;
	using size_type = typename VecT::size_type;
	using difference_type = typename VecT::difference_type;
	using reference = typename VecT::reference;
	using const_reference = typename VecT::const_reference;
	using pointer = typename VecT::pointer;
	using const_pointer = typename VecT::const_pointer;
	using iterator = typename VecT::iterator;
	using const_iterator = typename VecT::const_iterator;
	using reverse_iterator = typename VecT::reverse_iterator;
	using const_reverse_iterator = typename VecT::const_reverse_iterator;

	List_Basic_Strings() = default;
	explicit List_Basic_Strings(size_type n) : d(n), sz(n) {}
	List_Basic_Strings(size_type n, const_reference value)
	    : d(n, value), sz(n)
	{
	}
	template <class InputIterator>
	List_Basic_Strings(InputIterator first, InputIterator last)
	    : d(first, last), sz(d.size())
	{
	}
	List_Basic_Strings(std::initializer_list<value_type> il)
	    : d(il), sz(d.size())
	{
	}

	List_Basic_Strings(const List_Basic_Strings& other) = default;
	List_Basic_Strings(List_Basic_Strings&& other)
	    : d(move(other.d)), sz(other.sz)
	{
		other.sz = 0;
	}

	List_Basic_Strings(VecT&& other) : d(other), sz(other.size()) {}

	auto& operator=(const List_Basic_Strings& other)
	{
		clear();
		insert(begin(), other.begin(), other.end());
		return *this;
	}
	auto& operator=(List_Basic_Strings&& other)
	{
		d = move(other.d);
		sz = other.sz;
		other.sz = 0;
		return *this;
	}
	auto& operator=(std::initializer_list<value_type> il)
	{
		clear();
		insert(begin(), il);
		return *this;
	}
	template <class InputIterator>
	auto assign(InputIterator first, InputIterator last)
	{
		clear();
		insert(begin(), first, last);
	}
	void assign(size_type n, const_reference value)
	{
		clear();
		insert(begin(), n, value);
	}
	void assign(std::initializer_list<value_type> il) { *this = il; }
	auto get_allocator() const noexcept { return d.get_allocator(); }

	// iterators
	auto begin() noexcept -> iterator { return d.begin(); }
	auto begin() const noexcept -> const_iterator { return d.begin(); }
	auto end() noexcept -> iterator { return begin() + sz; }
	auto end() const noexcept -> const_iterator { return begin() + sz; }

	auto rbegin() noexcept { return d.rend() - sz; }
	auto rbegin() const noexcept { return d.rend() - sz; }
	auto rend() noexcept { return d.rend(); }
	auto rend() const noexcept { return d.rend(); }

	auto cbegin() const noexcept { return d.cbegin(); }
	auto cend() const noexcept { return cbegin() + sz; }

	auto crbegin() const noexcept { return d.crend() - sz; }
	auto crend() const noexcept { return d.crend(); }

	// [vector.capacity], capacity
	auto empty() const noexcept { return sz == 0; }
	auto size() const noexcept { return sz; }
	auto max_size() const noexcept { return d.max_size(); }
	auto capacity() const noexcept { return d.size(); }
	auto resize(size_type new_sz)
	{
		if (new_sz <= sz) {
		}
		else if (new_sz <= d.size()) {
			std::for_each(begin() + sz, begin() + new_sz,
			              [](auto& s) { s.clear(); });
		}
		else {
			std::for_each(d.begin() + sz, d.end(),
			              [](auto& s) { s.clear(); });
			d.resize(new_sz);
		}
		sz = new_sz;
	}
	auto resize(size_type new_sz, const_reference c)
	{
		if (new_sz <= sz) {
		}
		else if (new_sz <= d.size()) {
			std::for_each(begin() + sz, begin() + new_sz,
			              [&](auto& s) { s = c; });
		}
		else {
			std::for_each(d.begin() + sz, d.end(),
			              [&](auto& s) { s = c; });
			d.resize(new_sz, c);
		}
		sz = new_sz;
	}
	void reserve(size_type n)
	{
		if (n > d.size())
			d.resize(n);
	}
	void shrink_to_fit()
	{
		d.resize(sz);
		d.shrink_to_fit();
		for (auto& s : d) {
			s.shrink_to_fit();
		}
	}

	// element access
	auto& operator[](size_type n) { return d[n]; }
	auto& operator[](size_type n) const { return d[n]; }
	auto& at(size_type n)
	{
		if (n < sz)
			return d[n];
		else
			throw std::out_of_range("index is out of range");
	}
	auto& at(size_type n) const
	{
		if (n < sz)
			return d[n];
		else
			throw std::out_of_range("index is out of range");
	}
	auto& front() { return d.front(); }
	auto& front() const { return d.front(); }
	auto& back() { return d[sz - 1]; }
	auto& back() const { return d[sz - 1]; }

	// [vector.data], data access
	auto data() noexcept { return d.data(); }
	auto data() const noexcept { return d.data(); }

	// [vector.modifiers], modifiers
	template <class... Args>
	auto& emplace_back(Args&&... args)
	{
		if (sz != d.size())
			d[sz] = value_type(std::forward<Args>(args)...);
		else
			d.emplace_back(std::forward<Args>(args)...);
		return d[sz++];
	}
	auto& emplace_back()
	{
		if (sz != d.size())
			d[sz].clear();
		else
			d.emplace_back();
		return d[sz++];
	}
	auto push_back(const_reference x)
	{
		if (sz != d.size())
			d[sz] = x;
		else
			d.push_back(x);
		++sz;
	}
	auto push_back(value_type&& x)
	{
		if (sz != d.size())
			d[sz] = std::move(x);
		else
			d.push_back(std::move(x));
		++sz;
	}
	auto pop_back() { --sz; }

      private:
	template <class U>
	auto insert_priv(const_iterator pos, U&& val)
	{
		if (sz != d.size()) {
			d[sz] = std::forward<U>(val);
		}
		else {
			auto pos_idx = pos - cbegin();
			d.push_back(std::forward<U>(val));
			pos = cbegin() + pos_idx;
		}
		auto p = begin() + (pos - cbegin());
		std::rotate(p, begin() + sz, begin() + sz + 1);
		++sz;
		return p;
	}

      public:
	template <class... Args>
	auto emplace(const_iterator pos, Args&&... args)
	{
		if (sz != d.size()) {
			d[sz] = value_type(std::forward<Args>(args)...);
		}
		else {
			auto pos_idx = pos - cbegin();
			d.emplace(std::forward<Args>(args)...);
			pos = cbegin() + pos_idx;
		}
		auto p = begin() + (pos - cbegin());
		std::rotate(p, begin() + sz, begin() + sz + 1);
		++sz;
		return p;
	}
	auto insert(const_iterator pos, const_reference x)
	{
		return insert_priv(pos, x);
	}
	auto insert(const_iterator pos, value_type&& x)
	{
		return insert_priv(pos, std::move(x));
	}
	auto insert(const_iterator pos, size_type n, const_reference x)
	    -> iterator
	{
		auto i = sz;
		while (n != 0 && i != d.size()) {
			d[i] = x;
			--n;
			++i;
		}
		if (n != 0) {
			auto pos_idx = pos - cbegin();
			d.insert(d.end(), n, x);
			pos = cbegin() + pos_idx;
			i = d.size();
		}
		auto p = begin() + (pos - cbegin());
		std::rotate(p, begin() + sz, begin() + i);
		sz = i;
		return p;
	}

	template <class InputIterator>
	auto insert(const_iterator pos, InputIterator first, InputIterator last)
	    -> iterator
	{
		auto i = sz;
		while (first != last && i != d.size()) {
			d[i] = *first;
			++first;
			++i;
		}
		if (first != last) {
			auto pos_idx = pos - cbegin();
			d.insert(d.end(), first, last);
			pos = cbegin() + pos_idx;
			i = d.size();
		}
		auto p = begin() + (pos - cbegin());
		std::rotate(p, begin() + sz, begin() + i);
		sz = i;
		return p;
	}
	auto insert(const_iterator pos, std::initializer_list<value_type> il)
	    -> iterator
	{
		return insert(pos, il.begin(), il.end());
	}

	auto erase(const_iterator position)
	{
		auto i0 = begin();
		auto i1 = i0 + (position - cbegin());
		auto i2 = i1 + 1;
		auto i3 = end();
		std::rotate(i1, i2, i3);
		--sz;
		return i1;
	}
	auto erase(const_iterator first, const_iterator last)
	{
		auto i0 = begin();
		auto i1 = i0 + (first - cbegin());
		auto i2 = i0 + (last - cbegin());
		auto i3 = end();
		std::rotate(i1, i2, i3);
		sz -= last - first;
		return i1;
	}
	auto swap(List_Basic_Strings& other)
	{
		d.swap(other.d);
		std::swap(sz, other.sz);
	}
	auto clear() noexcept -> void { sz = 0; }

	auto operator==(const List_Basic_Strings& other) const
	{
		return std::equal(begin(), end(), other.begin(), other.end());
	}
	auto operator!=(const List_Basic_Strings& other) const
	{
		return !(*this == other);
	}
	auto operator<(const List_Basic_Strings& other) const
	{
		return std::lexicographical_compare(begin(), end(),
		                                    other.begin(), other.end());
	}
	auto operator>=(const List_Basic_Strings& other) const
	{
		return !(*this < other);
	}
	auto operator>(const List_Basic_Strings& other) const
	{
		return std::lexicographical_compare(other.begin(), other.end(),
		                                    begin(), end());
	}
	auto operator<=(const List_Basic_Strings& other) const
	{
		return !(*this > other);
	}

	auto extract_sequence() -> VecT
	{
		d.resize(sz);
		sz = 0;
		return std::move(d);
	}
};

template <class CharT>
auto swap(List_Basic_Strings<CharT>& a, List_Basic_Strings<CharT>& b)
{
	a.swap(b);
}

using List_Strings = List_Basic_Strings<char>;
using List_WStrings = List_Basic_Strings<wchar_t>;

template <class CharT>
class Replacement_Table {
      public:
	using StrT = std::basic_string<CharT>;
	using Table_Str = std::vector<std::pair<StrT, StrT>>;
	using iterator = typename Table_Str::iterator;
	using const_iterator = typename Table_Str::const_iterator;

      private:
	Table_Str table;
	size_t whole_word_reps_last_idx = 0;
	size_t start_word_reps_last_idx = 0;
	size_t end_word_reps_last_idx = 0;

	auto order_entries() -> void; // implemented in cxx

      public:
	Replacement_Table() = default;
	Replacement_Table(const Table_Str& v) : table(v) { order_entries(); }
	Replacement_Table(Table_Str&& v) : table(move(v)) { order_entries(); }

	auto& operator=(const Table_Str& v)
	{
		table = v;
		order_entries();
		return *this;
	}

	auto& operator=(Table_Str&& v)
	{
		table = move(v);
		order_entries();
		return *this;
	}

	template <class Range>
	auto& operator=(const Range& range)
	{
		table.assign(std::begin(range), std::end(range));
		order_entries();
		return *this;
	}

	auto whole_word_replacements() const
	    -> boost::iterator_range<const_iterator>
	{
		return {begin(table), begin(table) + whole_word_reps_last_idx};
	}
	auto start_word_replacements() const
	    -> boost::iterator_range<const_iterator>
	{
		return {begin(table) + whole_word_reps_last_idx,
		        begin(table) + start_word_reps_last_idx};
	}
	auto end_word_replacements() const
	    -> boost::iterator_range<const_iterator>
	{
		return {begin(table) + start_word_reps_last_idx,
		        begin(table) + end_word_reps_last_idx};
	}
	auto any_place_replacements() const
	    -> boost::iterator_range<const_iterator>
	{
		return {begin(table) + end_word_reps_last_idx, end(table)};
	}
};
template <class CharT>
auto Replacement_Table<CharT>::order_entries() -> void
{
	auto it = remove_if(begin(table), end(table), [](auto& p) {
		auto& s = p.first;
		return s.empty() ||
		       (s.size() == 1 && (s[0] == '^' || s[0] == '$'));
	});
	table.erase(it, end(table));

	auto is_start_word_pat = [=](auto& x) { return x.first[0] == '^'; };
	auto is_end_word_pat = [=](auto& x) { return x.first.back() == '$'; };

	auto start_word_reps_last =
	    partition(begin(table), end(table), is_start_word_pat);
	start_word_reps_last_idx = start_word_reps_last - begin(table);
	for_each(begin(table), start_word_reps_last,
	         [](auto& e) { e.first.erase(0, 1); });

	auto whole_word_reps_last =
	    partition(begin(table), start_word_reps_last, is_end_word_pat);
	whole_word_reps_last_idx = whole_word_reps_last - begin(table);
	for_each(begin(table), whole_word_reps_last,
	         [](auto& e) { e.first.pop_back(); });

	auto end_word_reps_last =
	    partition(start_word_reps_last, end(table), is_end_word_pat);
	end_word_reps_last_idx = end_word_reps_last - begin(table);
	for_each(start_word_reps_last, end_word_reps_last,
	         [](auto& e) { e.first.pop_back(); });
}

template <class CharT>
struct Similarity_Group {
	using StrT = std::basic_string<CharT>;
	StrT chars;
	std::vector<StrT> strings;

	auto parse(const StrT& s) -> void;
	Similarity_Group() = default;
	Similarity_Group(const StrT& s) { parse(s); }
	auto& operator=(const StrT& s)
	{
		parse(s);
		return *this;
	}
};
template <class CharT>
auto Similarity_Group<CharT>::parse(const StrT& s) -> void
{
	auto i = size_t(0);
	for (;;) {
		auto j = s.find('(', i);
		chars.append(s, i, j - i);
		if (j == s.npos)
			break;
		i = j + 1;
		j = s.find(')', i);
		if (j == s.npos)
			break;
		auto len = j - i;
		if (len == 1)
			chars += s[i];
		else if (len > 1)
			strings.push_back(s.substr(i, len));
		i = j + 1;
	}
}

template <class CharT>
class Phonetic_Table {
	using StrT = std::basic_string<CharT>;
	using Pair_StrT = std::pair<StrT, StrT>;

	struct Phonet_Match_Result {
		size_t count_matched = 0;
		size_t go_back_before_replace = 0;
		size_t priority = 5;
		bool go_back_after_replace = false;
		bool treat_next_as_begin = false;
		operator bool() { return count_matched; }
	};

	std::vector<std::pair<StrT, StrT>> table;
	auto order() -> void;
	auto static match(const StrT& data, size_t i, const StrT& pattern,
	                  bool at_begin) -> Phonet_Match_Result;

      public:
	Phonetic_Table() = default;
	Phonetic_Table(const std::vector<Pair_StrT>& v) : table(v) { order(); }
	Phonetic_Table(std::vector<Pair_StrT>&& v) : table(move(v)) { order(); }
	auto& operator=(const std::vector<Pair_StrT>& v)
	{
		table = v;
		order();
		return *this;
	}
	auto& operator=(std::vector<Pair_StrT>&& v)
	{
		table = move(v);
		order();
		return *this;
	}
	template <class Range>
	auto& operator=(const Range& range)
	{
		table.assign(std::begin(range), std::end(range));
		order();
		return *this;
	}
	auto replace(StrT& word) const -> bool;
};

template <class CharT>
auto Phonetic_Table<CharT>::order() -> void
{
	stable_sort(begin(table), end(table), [](auto& pair1, auto& pair2) {
		if (pair2.first.empty())
			return false;
		if (pair1.first.empty())
			return true;
		return pair1.first[0] < pair2.first[0];
	});
	auto it = find_if_not(begin(table), end(table),
	                      [](auto& p) { return p.first.empty(); });
	table.erase(begin(table), it);
	for (auto& r : table) {
		if (r.second == NUSPELL_LITERAL(CharT, "_"))
			r.second.clear();
	}
}

template <class CharT>
auto Phonetic_Table<CharT>::match(const StrT& data, size_t i,
                                  const StrT& pattern, bool at_begin)
    -> Phonet_Match_Result
{
	auto ret = Phonet_Match_Result();
	auto j =
	    pattern.find_first_of(NUSPELL_LITERAL(CharT, "(<-0123456789^$"));
	if (j == pattern.npos)
		j = pattern.size();
	if (data.compare(i, j, pattern, 0, j) == 0)
		ret.count_matched = j;
	else
		return {};
	if (j == pattern.size())
		return ret;
	if (pattern[j] == '(') {
		auto k = pattern.find(')', j);
		if (k == pattern.npos)
			return {}; // bad rule
		auto x = std::char_traits<CharT>::find(
		    &pattern[j + 1], k - (j + 1), data[i + j]);
		if (!x)
			return {};
		j = k + 1;
		ret.count_matched += 1;
	}
	if (j == pattern.size())
		return ret;
	if (pattern[j] == '<') {
		ret.go_back_after_replace = true;
		++j;
	}
	auto k = pattern.find_first_not_of('-', j);
	if (k == pattern.npos) {
		k = pattern.size();
		ret.go_back_before_replace = k - j;
		if (ret.go_back_before_replace >= ret.count_matched)
			return {}; // bad rule
		return ret;
	}
	else {
		ret.go_back_before_replace = k - j;
		if (ret.go_back_before_replace >= ret.count_matched)
			return {}; // bad rule
	}
	j = k;
	if (pattern[j] >= '0' && pattern[j] <= '9') {
		ret.priority = pattern[j] - '0';
		++j;
	}
	if (j == pattern.size())
		return ret;
	if (pattern[j] == '^') {
		if (!at_begin)
			return {};
		++j;
	}
	if (j == pattern.size())
		return ret;
	if (pattern[j] == '^') {
		ret.treat_next_as_begin = true;
		++j;
	}
	if (j == pattern.size())
		return ret;
	if (pattern[j] != '$')
		return {}; // bad rule, no other char is allowed at this point
	if (i + ret.count_matched == data.size())
		return ret;
	return {};
}

template <class CharT>
auto Phonetic_Table<CharT>::replace(StrT& word) const -> bool
{
	using boost::make_iterator_range;
	struct Cmp {
		auto operator()(CharT c, const Pair_StrT& s)
		{
			return c < s.first[0];
		}
		auto operator()(const Pair_StrT& s, CharT c)
		{
			return s.first[0] < c;
		}
	};
	if (table.empty())
		return false;
	auto ret = false;
	auto treat_next_as_begin = true;
	size_t count_go_backs_after_replace = 0; // avoid infinite loop
	for (size_t i = 0; i != word.size(); ++i) {
		auto rules =
		    equal_range(begin(table), end(table), word[i], Cmp());
		for (auto& r : make_iterator_range(rules)) {
			auto rule = &r;
			auto m1 = match(word, i, r.first, treat_next_as_begin);
			if (!m1)
				continue;
			if (!m1.go_back_before_replace) {
				auto j = i + m1.count_matched - 1;
				auto rules2 = equal_range(
				    begin(table), end(table), word[j], Cmp());
				for (auto& r2 : make_iterator_range(rules2)) {
					auto m2 =
					    match(word, j, r2.first, false);
					if (m2 && m2.priority >= m1.priority) {
						i = j;
						rule = &r2;
						m1 = m2;
						break;
					}
				}
			}
			word.replace(
			    i, m1.count_matched - m1.go_back_before_replace,
			    rule->second);
			treat_next_as_begin = m1.treat_next_as_begin;
			if (m1.go_back_after_replace &&
			    count_go_backs_after_replace < 100) {
				count_go_backs_after_replace++;
			}
			else {
				i += rule->second.size();
			}
			--i;
			ret = true;
			break;
		}
	}
	return ret;
}
} // namespace nuspell
#endif // NUSPELL_STRUCTURES_HXX
