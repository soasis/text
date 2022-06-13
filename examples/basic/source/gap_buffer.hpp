// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

// A gap buffer implementation taken right off of GitHub
// and modified to compile with compilers from this decade.
// — https://github.com/crfh/gap_vector

/* gap_vector コンテナ
 * このファイルは public-domain とします
 */

/* 特徴
 * この実装では、例外安全性についてかなり注意を払っています。
 * また、C++11対応のためのコードも含まれています。
 *
 * 注意
 * 設定が必要です。
 * * NDEBUG // これを#defineすると、デバッグコードが省かれます。
 * * __GAP_CXX11__ // これを1にすると、c++11用のコードが展開されます  gcc 4.5に関しては、自動でこれを行います
 * * std::_Destroy // 区間に対してallocator.destroyを呼び出す関数です
 *
 */

#ifndef GAP_VECTOR_HPP
#define GAP_VECTOR_HPP

#include <new>
#include <iterator>
#include <stdexcept>
#include <memory>
#ifndef NDEBUG
#include <iostream>
#endif

#define __GAP_CXX11__ 1

#if __GAP_CXX11__
#include <initializer_list>
#endif

#if __GAP_CXX11__
#define GAP_MAKE_MOVE_ITER(I) std::make_move_iterator((I))
#define GAP_FORWARD(A) std::forward<Args>((A))...
#else
#define GAP_MAKE_MOVE_ITER(I) (I)
#define GAP_FORWARD(A) (A)
#endif

namespace gap {

	// 下は uninitialized に含まれない
	namespace {
		template <typename ForwardIter, typename Alloc>
		void Destroy(ForwardIter first, ForwardIter last, Alloc& alloc) {
			for (; first != last; ++first) {
				std::allocator_traits<Alloc>::destroy(alloc, &*(first));
			}
		}

		template <typename InputIter, typename ForwardIter, typename Alloc>
		ForwardIter uninitialized_copy_a(InputIter first, InputIter last, ForwardIter result, Alloc& alloc) {
			ForwardIter t = result;
			try {
				for (; first != last; ++first) {
					std::allocator_traits<Alloc>::construct(alloc, &*(result), *first);
					++result;
				}
			}
			catch (...) {
				Destroy(t, result, alloc);
				throw;
			}
			return result;
		}

		template <typename ForwardIter, typename size_type, typename Tp, typename Alloc>
		void uninitialized_fill_n_a(ForwardIter first, size_type sz, const Tp& value, Alloc& alloc) {
			ForwardIter t = first;
			try {
				for (; sz != 0; --sz) {
					std::allocator_traits<Alloc>::construct(alloc, &*(first), value);
					++first;
				}
			}
			catch (...) {
				Destroy(t, first, alloc);
			}
		}
	} // namespace


	// gapのイテレータは、速くはない
	namespace {
		template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
		struct gap_iterator_base {
		public:
			typedef Tp value_type;
			typedef DiffTp difference_type;
			typedef PtrTp pointer;
			typedef RefTp reference;
			typedef gap_iterator_base Self;
			typedef std::random_access_iterator_tag iterator_category;
			typedef std::random_access_iterator_tag iterator_concept;

			gap_iterator_base() {
			}
			gap_iterator_base(pointer c, pointer b, pointer e) : cur(c), gap_begin(b), gap_end(e) {
			}
			gap_iterator_base(const Self& rhs) : cur(rhs.cur), gap_begin(rhs.gap_begin), gap_end(rhs.gap_end) {
			}
			Self& operator=(const Self& rhs) {
				cur       = rhs.cur;
				gap_begin = rhs.gap_begin;
				gap_end   = rhs.gap_end;
				return *this;
			}

			Self operator+(difference_type n) const {
				if (cur < gap_begin && cur + n >= gap_begin)
					return Self(cur + n + (gap_end - gap_begin), gap_begin, gap_end);
				if (cur >= gap_end && cur + n < gap_end)
					return Self(cur + n - (gap_end - gap_begin), gap_begin, gap_end);
				return Self(cur + n, gap_begin, gap_end);
			}
			Self operator-(difference_type n) const {
				return this->operator+(-n);
			}
			difference_type operator-(const Self& rhs) const {
				if (cur < rhs.cur) {
					if (cur > gap_begin || rhs.cur < gap_begin)
						return rhs.cur - cur;
					else
						return rhs.cur - cur - (gap_end - gap_begin);
				}
				else {
					if (rhs.cur > gap_begin || cur < gap_begin)
						return cur - rhs.cur;
					else
						return cur - rhs.cur - (gap_end - gap_begin);
				}
			}
			Self& operator++() {
				cur++;
				cur = cur == gap_begin ? cur + (gap_end - gap_begin) : cur;
				return *this;
			}
			Self operator++(int) {
				Self itr = *this;
				++*this;
				return itr;
			}
			Self& operator--() {
				cur = cur == gap_end ? cur - (gap_end - gap_begin) - 1 : cur - 1;
				return *this;
			}
			Self operator--(int) {
				Self itr = *this;
				--*this;
				return itr;
			}
			Self& operator+=(difference_type n) {
				return (*this = *this + n);
			}
			Self& operator-=(difference_type n) {
				return (*this = *this - n);
			}

			reference operator*() const {
				return *cur;
			}
			pointer operator->() const {
				return cur;
			}

			operator pointer() const {
				return cur;
			}

			pointer cur, gap_begin, gap_end;
		};
	} // namespace

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline bool operator==(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return lhs.cur == rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp, typename RDiffTp, typename RPtrTp,
	     typename RRefTp>
	inline bool operator==(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, RDiffTp, RPtrTp, RRefTp>& rhs) {
		return lhs.cur == rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline bool operator!=(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return lhs.cur != rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp, typename RDiffTp, typename RPtrTp,
	     typename RRefTp>
	inline bool operator!=(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, RDiffTp, RPtrTp, RRefTp>& rhs) {
		return lhs.cur != rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline bool operator<(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return lhs.cur < rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp, typename RDiffTp, typename RPtrTp,
	     typename RRefTp>
	inline bool operator<(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, RDiffTp, RPtrTp, RRefTp>& rhs) {
		return lhs.cur < rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline bool operator<=(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return lhs.cur <= rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp, typename RDiffTp, typename RPtrTp,
	     typename RRefTp>
	inline bool operator<=(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, RDiffTp, RPtrTp, RRefTp>& rhs) {
		return lhs.cur <= rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline bool operator>(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return lhs.cur > rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp, typename RDiffTp, typename RPtrTp,
	     typename RRefTp>
	inline bool operator>(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, RDiffTp, RPtrTp, RRefTp>& rhs) {
		return lhs.cur > rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline bool operator>=(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return lhs.cur >= rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp, typename RDiffTp, typename RPtrTp,
	     typename RRefTp>
	inline bool operator>=(const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& lhs,
	     const gap_iterator_base<Tp, RDiffTp, RPtrTp, RRefTp>& rhs) {
		return lhs.cur >= rhs.cur;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline gap_iterator_base<Tp, DiffTp, PtrTp, RefTp> operator+(
	     DiffTp lhs, const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return rhs + lhs;
	}

	template <typename Tp, typename DiffTp, typename PtrTp, typename RefTp>
	inline gap_iterator_base<Tp, DiffTp, PtrTp, RefTp> operator-(
	     DiffTp lhs, const gap_iterator_base<Tp, DiffTp, PtrTp, RefTp>& rhs) {
		return rhs - lhs;
	}

	// gap_vectorの例外安全性について
	// gap_vectorでは、様々な操作で、内部データのコピーが発生する
	// その結果、格納する型の例外安全性が、gap_vectorの例外安全性にとても関係する
	// もし格納する型のデストラクタが例外を送出するなら、gap_vectorは例外安全でない
	// そうでない場合
	//   もし格納する型のムーブコンストラクタが例外を送出するなら、gap_vectorは基本例外保証をする
	//   一方で、格納する型のムーブコンストラクタが例外を送出しないなら、gap_vectorは、強い例外安全保証をする
	//   もし格納する型にムーブコンストラクタが定義されていない場合、以下のケースとなる
	//     もし格納する型のコピーコンストラクタが例外を送出するなら、gap_vectorは、基本例外安全保証をする
	//     もし格納する型のコピーコンストラクタが例外を送出しないなら、gap_vectorは、強い例外安全保証をする
	// 詳細はそれぞれのメンバ関数宣言を参照のこと(書いていないものは、上のルールに従っている)
	// また、constでない操作の場合、ほとんど確実にiteratorが破壊されることに注意
	// vectorでは、ときどき破壊されるぐらいだが、gap_vectorでは、ほぼ間違いなくいつでも破壊される

	// 基本的に、コピーやムーブで例外を出すようなオブジェクトは、unique_ptrに入れるべき

	// インターフェースはvectorといっしょ
	template <typename Tp, typename Alloc = std::allocator<Tp>>
	class gap_vector {
	private:
		typedef std::allocator_traits<Alloc> alloc_traits;

	public:
		typedef typename alloc_traits::template rebind_alloc<Tp> alloc_type;
		typedef Tp value_type;
		typedef typename alloc_traits::pointer pointer;
		typedef typename alloc_traits::const_pointer const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename alloc_traits::size_type size_type;
		typedef typename alloc_traits::difference_type difference_type;
		typedef gap_iterator_base<value_type, difference_type, pointer, reference> iterator;
		typedef gap_iterator_base<value_type, difference_type, const_pointer, const_reference> const_iterator;
		typedef typename std::reverse_iterator<iterator> reverse_iterator;
		typedef typename std::reverse_iterator<const_iterator> const_reverse_iterator;


		gap_vector() : bbegin(0), bend(0), gap_begin(0), gap_end(0) {
		}

		explicit gap_vector(const alloc_type& alloc)
		: bbegin(0), bend(0), gap_begin(0), gap_end(0), allocator(alloc) {
		}

		explicit gap_vector(size_type n, const value_type& v = value_type(), const alloc_type& alloc = alloc_type())
		: allocator(alloc) {
			bbegin = allocator.allocate(n);
			bend = gap_begin = gap_end = bbegin + n;
			try {
				uninitialized_fill_n_a(bbegin, n, v, allocator);
			}
			catch (...) {
				allocator.deallocate(bbegin, n);
				throw;
			}
		}

		template <typename InputIter>
		gap_vector(InputIter first, InputIter last) : bbegin(0), bend(0), gap_begin(0), gap_end(0) {
			try {
				insert(begin(), first, last);
			}
			catch (...) {
				m_release_buf();
				throw;
			}
		}

		gap_vector(const gap_vector& rhs) : allocator(rhs.allocator) {
			size_type sz = rhs.size();
			pointer buf  = allocator.allocate(sz);
			try {
				rhs.m_copy_to_buf(buf, allocator);
			}
			catch (...) {
				allocator.deallocate(buf, sz);
				throw;
			}
			bbegin = buf;
			bend = gap_begin = gap_end = buf + sz;
		}

#if __GAP_CXX11__
		// ムーブコンストラクタは例外を送出しない
		gap_vector(gap_vector&& rhs)
		: bbegin(rhs.bbegin)
		, bend(rhs.bend)
		, gap_begin(rhs.gap_begin)
		, gap_end(rhs.gap_end)
		, allocator(std::move(rhs.allocator)) {
			rhs.bbegin = rhs.bend = rhs.gap_begin = rhs.gap_end = 0;
		}

		gap_vector(std::initializer_list<value_type> init, const alloc_type& alloc = alloc_type())
		: bbegin(0), bend(0), gap_begin(0), gap_end(0), allocator(alloc) {
			try {
				m_range_insert(begin(), init.begin(), init.end(), std::random_access_iterator_tag());
			}
			catch (...) {
				m_release_buf();
				throw;
			}
		}
#endif

		~gap_vector() {
			m_release_buf();
		}

		gap_vector& operator=(const gap_vector& rhs) {
			// データのコピー成功後にバッファの解放を行う
			size_type sz = rhs.size();
			pointer buf  = allocator.allocate(sz);
			try {
				rhs.m_copy_to_buf(buf, allocator);
			}
			catch (...) {
				allocator.deallocate(buf, sz);
				throw;
			}
			m_release_buf();
			bbegin = buf;
			bend = gap_begin = gap_end = buf + sz;
			return *this;
		}
#if __GAP_CXX11__
		// ムーブは例外を送出しない
		gap_vector& operator=(gap_vector&& rhs) {
			m_release_buf();
			bbegin     = rhs.bbegin;
			bend       = rhs.bend;
			gap_begin  = rhs.gap_begin;
			gap_end    = rhs.gap_end;
			rhs.bbegin = rhs.bend = rhs.gap_begin = rhs.gap_end = 0;
			return *this;
		}
#endif


#if !__GAP_CXX11__
		// insert後は、itrにあった要素がgapの直後にくるようになる
		iterator insert(const iterator& itr, const value_type& e) {
			return m_emplace(itr, e);
		}
#else
		iterator insert(const iterator& itr, const value_type& e) {
			value_type t = e;
			return m_emplace(itr, std::move(t));
		}

		iterator insert(const iterator& itr, value_type&& e) {
			return emplace(itr, std::move(e));
		}

		template <typename... Args>
		iterator emplace(const iterator& itr, Args&&... args) {
			return m_emplace(itr, std::forward<Args>(args)...);
		}
#endif


		template <typename InputIter>
		void insert(const iterator& itr, InputIter first, InputIter last) {
			typedef typename std::iterator_traits<InputIter>::iterator_category IterCategory;
			m_range_insert(itr, first, last, IterCategory());
		}


		// itr の位置の要素を削除し、gapの位置をitrのあった場所から始まるように移動する
		void erase(const iterator& itr) {
			pointer ptr = itr;
			if (gap_begin < ptr) {
				m_move_rtl(ptr);
				allocator.destroy(gap_end++);
			}
			else {
				m_move_ltr(++ptr);
				allocator.destroy(--gap_begin);
			}
		}

		void erase(const iterator& first, const iterator& last) {
			pointer f = first, l = last;
			if (f > gap_end) {
				m_move_rtl(f); // fより前を移動
				Destroy(f, l, allocator);
				gap_end = l;
			}
			else if (l <= gap_end) {
				if (l == gap_end)
					l = gap_begin;
				else
					m_move_ltr(l); // lを含めて移動
				Destroy(f, l, allocator);
				gap_begin = f;
			}
			else {
				Destroy(f, gap_begin, allocator);
				Destroy(gap_end, l, allocator);
				gap_begin = f;
				gap_end   = l;
			}
		}

#if !__GAP_CXX11__
		// push_back insert( end(), e ) と同じ
		void push_back(const value_type& e) {
			m_emplace_back(e);
		}
#else
		void push_back(const value_type& e) {
			value_type t = e;
			m_emplace_back(std::move(t));
		}

		template <typename... Args>
		void emplace_back(Args&&... args) {
			m_emplace_back(std::forward<Args>(args)...);
		}

		void push_back(value_type&& e) {
			m_emplace_back(std::move(e));
		}
#endif

		void pop_back() {
			if (bend == gap_end)
				allocator.destroy(--gap_begin);
			else
				erase(--end());
		}

#if !__GAP_CXX11__
		void push_front(const value_type& e) {
			m_emplace_front(e);
		}
#else
		void push_front(const value_type& e) {
			value_type t = e;
			m_emplace_front(std::move(t));
		}

		template <typename... Args>
		void emplace_front(Args&&... args) {
			m_emplace_front(std::forward<Args>(args)...);
		}

		void push_front(value_type&& e) {
			m_emplace_front(std::move(e));
		}
#endif

		void pop_front() {
			if (gap_begin == bbegin)
				allocator.destroy(gap_end++);
			else
				erase(begin());
		}

		// gapの幅を広げる  つまり、gapの論理的な位置を変えない
		void reserve(size_type n) {
			if (n >= max_size())
				throw std::length_error("gap_vector::reserve");
			if (capacity() < n) {
				size_type sz = size() + n;
				pointer buf  = allocator.allocate(sz);
				try {
					m_move_to_newbuf(buf, sz);
				}
				catch (...) {
					allocator.deallocate(buf, sz);
					throw;
				}
			}
		}

		// shrink_to_fitはc++98でも実現でき、便利なので、どちらでも使えるようにしている
		// cbegin, cend もそう
		void shrink_to_fit() {
			size_type sz = size();
			pointer buf  = allocator.allocate(sz);
			try {
				m_move_to_newbuf(buf, sz);
			}
			catch (...) {
				allocator.deallocate(buf);
				throw;
			}
		}

		void clear() {
			Destroy(bbegin, gap_begin);
			gap_begin = bbegin;
			Destroy(gap_end, bend);
			gap_end = bend;
		}

		void swap(gap_vector& rhs) {
			using std::swap;
			swap(bbegin, rhs.bbegin);
			swap(bend, rhs.bend);
			swap(gap_begin, rhs.gap_begin);
			swap(gap_end, rhs.gap_end);
			swap(allocator, rhs.allocator);
		}

		size_type max_size() const {
			return allocator.max_size();
		}

		size_type capacity() const {
			return gap_end - gap_begin;
		}

		bool empty() const {
			return bbegin == gap_begin && bend == gap_end;
		}

		size_type size() const {
			return (bend - gap_end) + (gap_begin - bbegin);
		}

		iterator end() {
			return gap_end == bend ? iterator(gap_begin, gap_begin, gap_end) : iterator(bend, gap_begin, gap_end);
		}
		iterator begin() {
			return bbegin == gap_begin ? iterator(gap_end, gap_begin, gap_end)
			                           : iterator(bbegin, gap_begin, gap_end);
		}

		const_iterator end() const {
			return cend();
		}
		const_iterator begin() const {
			return cbegin();
		}
		const_iterator cend() const {
			return gap_end == bend ? const_iterator(gap_begin, gap_begin, gap_end)
			                       : const_iterator(bend, gap_begin, gap_end);
		}
		const_iterator cbegin() const {
			return bbegin == gap_begin ? const_iterator(gap_end, gap_begin, gap_end)
			                           : const_iterator(bbegin, gap_begin, gap_end);
		}

		reverse_iterator rend() {
			return reverse_iterator(begin());
		}
		reverse_iterator rbegin() {
			return reverse_iterator(end());
		}

		const_reverse_iterator rend() const {
			return const_reverse_iterator(cbegin());
		}
		const_reverse_iterator rbegin() const {
			return const_reverse_iterator(cend());
		}
		const_reverse_iterator crend() const {
			return const_reverse_iterator(cbegin());
		}
		const_reverse_iterator crbegin() const {
			return const_reverse_iterator(cend());
		}

		reference front() {
			return gap_begin == bbegin ? *bend : *bbegin;
		}
		const_reference front() const {
			return gap_begin == bbegin ? *bend : *bbegin;
		}
		reference back() {
			return gap_end == bend ? *(gap_begin - 1) : *(bend - 1);
		}
		const_reference back() const {
			return gap_end == bend ? *(gap_begin - 1) : *(bend - 1);
		}

		reference operator[](size_type indx) {
			if (bbegin + indx >= gap_begin) {
				return *(gap_end + (bbegin + indx - gap_begin));
			}
			else
				return *(bbegin + indx);
		}
		const_reference operator[](size_type indx) const {
			if (bbegin + indx >= gap_begin) {
				return *(gap_end + (bbegin + indx - gap_begin));
			}
			else
				return *(bbegin + indx);
		}

		reference at(size_type indx) {
			if (size() <= indx)
				throw std::out_of_range("gap_vector::at");
			if (bbegin + indx >= gap_begin) {
				return *(gap_end + (bbegin + indx - gap_begin));
			}
			else
				return *(bbegin + indx);
		}
		const_reference at(size_type indx) const {
			if (size() <= indx)
				throw std::out_of_range("gap_vector::at");
			if (bbegin + indx >= gap_begin) {
				return *(gap_end + (bbegin + indx - gap_begin));
			}
			else
				return *(bbegin + indx);
		}

#ifndef NDEBUG
		void print() const {
			for (pointer ptr = bbegin; ptr != gap_begin; ++ptr)
				std::cout << *ptr << ',';
			for (pointer ptr = gap_begin; ptr != gap_end; ++ptr)
				std::cout << '_' << ',';
			for (pointer ptr = gap_end; ptr != bend; ++ptr)
				std::cout << *ptr << ',';
			std::cout << std::endl;
		}
#endif

	private:
		pointer bbegin;    // メモリ領域の先頭
		pointer bend;      // メモリ領域の末尾  [bbegin,bend)
		pointer gap_begin; // gap の先頭
		pointer gap_end;   // gap の末尾 [gap_begin,gap_end)
		alloc_type allocator;

	private:
		// 2倍伸長ルールで、新しい領域の大きさを決める
		size_type m_next_size() const {
			return (bend - bbegin) ? ((bend - bbegin) << 1) : 1;
		}

		// 新しいバッファへ全要素を転送する
		void m_move_to_newbuf(pointer buf, size_type sz) {
			uninitialized_copy_a(GAP_MAKE_MOVE_ITER(bbegin), GAP_MAKE_MOVE_ITER(gap_begin), buf, allocator);
			try {
				uninitialized_copy_a(
				     GAP_MAKE_MOVE_ITER(gap_end), GAP_MAKE_MOVE_ITER(bend), buf + sz - (bend - gap_end), allocator);
			}
			catch (...) {
				Destroy(buf, gap_begin - bbegin + buf, allocator);
				throw;
			}
			m_release_buf();
			gap_begin = gap_begin - bbegin + buf;
			bbegin    = buf;
			gap_end   = buf + sz - (bend - gap_end);
			bend      = buf + sz;
		}

		// 新しいgap位置を受け取るバージョン
		void m_move_to_newbuf(pointer buf, pointer g, size_type sz) {
			if (g < gap_end) {
				size_type r1      = g - bbegin;
				size_type r2      = gap_begin - g;
				size_type r3      = bend - gap_end;
				pointer nwgap_end = buf + sz - r2 - r3;
				uninitialized_copy_a(GAP_MAKE_MOVE_ITER(bbegin), GAP_MAKE_MOVE_ITER(g), buf, allocator);
				try {
					uninitialized_copy_a(
					     GAP_MAKE_MOVE_ITER(g), GAP_MAKE_MOVE_ITER(gap_begin), nwgap_end, allocator);
				}
				catch (...) {
					Destroy(buf, buf + r1, allocator);
					throw;
				}
				try {
					uninitialized_copy_a(
					     GAP_MAKE_MOVE_ITER(gap_end), GAP_MAKE_MOVE_ITER(bend), nwgap_end + r2, allocator);
				}
				catch (...) {
					Destroy(buf, buf + r1, allocator);
					Destroy(nwgap_end, nwgap_end + r2, allocator);
					throw;
				}
				m_release_buf();
				bbegin    = buf;
				gap_begin = buf + r1;
				gap_end   = nwgap_end;
				bend      = buf + sz;
			}
			else {
				size_type r1      = gap_begin - bbegin;
				size_type r2      = g - gap_end;
				size_type r3      = bend - g;
				pointer nwgap_end = buf + sz - r3;
				uninitialized_copy_a(GAP_MAKE_MOVE_ITER(bbegin), GAP_MAKE_MOVE_ITER(gap_begin), buf, allocator);
				try {
					uninitialized_copy_a(GAP_MAKE_MOVE_ITER(gap_end), GAP_MAKE_MOVE_ITER(g), buf + r1, allocator);
				}
				catch (...) {
					Destroy(buf, buf + r1, allocator);
					throw;
				}
				try {
					uninitialized_copy_a(GAP_MAKE_MOVE_ITER(g), GAP_MAKE_MOVE_ITER(bend), nwgap_end, allocator);
				}
				catch (...) {
					Destroy(buf, buf + r1, allocator);
					Destroy(nwgap_end, nwgap_end + r2, allocator);
					throw;
				}
				m_release_buf();
				bbegin    = buf;
				gap_begin = buf + r1 + r2;
				gap_end   = nwgap_end;
				bend      = buf + sz;
			}
		}

		// shrink_to_fitで未初期化領域へデータをコピーする
		void m_copy_to_buf(pointer buf, alloc_type& al) const {
			uninitialized_copy_a(bbegin, gap_begin, buf, al);
			try {
				uninitialized_copy_a(gap_end, bend, buf + (gap_begin - bbegin), al);
			}
			catch (...) {
				Destroy(buf, buf + (gap_begin - bbegin), al);
				throw;
			}
		}

		void m_release_buf() {
			if (bbegin != 0) {
				Destroy(bbegin, gap_begin, allocator);
				Destroy(gap_end, bend, allocator);
				allocator.deallocate(bbegin, bend - bbegin);
			}
		}

		// [gap_end, ptr) を、gap_beginへ移動する
		void m_move_rtl(pointer ptr) {
			pointer nwgap_end = gap_begin + (ptr - gap_end);
			if (nwgap_end < gap_end) {
				gap_begin = uninitialized_copy_a(
				     GAP_MAKE_MOVE_ITER(gap_end), GAP_MAKE_MOVE_ITER(ptr), gap_begin, allocator);
				Destroy(gap_end, ptr, allocator);
				gap_end = ptr;
			}
			else {
				size_type un = gap_end - gap_begin;
				uninitialized_copy_a(
				     GAP_MAKE_MOVE_ITER(gap_end), GAP_MAKE_MOVE_ITER(gap_end + un), gap_begin, allocator);
				try {
					std::copy(GAP_MAKE_MOVE_ITER(gap_end + un), GAP_MAKE_MOVE_ITER(ptr), gap_begin + un);
				}
				catch (...) {
					Destroy(gap_begin, gap_begin + un, allocator);
					throw;
				}
				Destroy(gap_begin + (ptr - gap_end), ptr, allocator);
				gap_begin = gap_begin + (ptr - gap_end);
				gap_end   = ptr;
			}
		}

		// [ptr, gap_begin) を、gap_endの直前へ移動する
		void m_move_ltr(pointer ptr) {
			size_type mvn     = gap_begin - ptr; // 移動する個数
			pointer nwgap_end = gap_end - mvn;   // 新しい gap_end;
			if (nwgap_end < gap_begin) {
				size_type un = gap_end - gap_begin;
				uninitialized_copy_a(
				     GAP_MAKE_MOVE_ITER(gap_begin - un), GAP_MAKE_MOVE_ITER(gap_begin), gap_end - un, allocator);
				try {
					std::copy_backward(GAP_MAKE_MOVE_ITER(ptr), GAP_MAKE_MOVE_ITER(gap_begin - un), gap_end - un);
				}
				catch (...) {
					Destroy(gap_end - un, gap_end, allocator);
					throw;
				}
				Destroy(ptr, nwgap_end, allocator);
			}
			else {
				uninitialized_copy_a(
				     GAP_MAKE_MOVE_ITER(ptr), GAP_MAKE_MOVE_ITER(gap_begin), gap_end - mvn, allocator);
				Destroy(ptr, gap_begin, allocator);
			}
			gap_begin = ptr;
			gap_end   = nwgap_end;
		}


		// gapの位置をitrの直前に変える
		void m_make_gap(const iterator& itr) {
			pointer ptr = itr;
			if (gap_begin < ptr)
				m_move_rtl(ptr);
			else
				m_move_ltr(ptr);
		}

#ifdef __GAP_CXX11__
		template <typename... Args>
		iterator m_emplace(const iterator& itr, Args&&... args)
#else
		iterator m_emplace(const iterator& itr, const value_type& args)
#endif
		{
			if (capacity()) {
				m_make_gap(itr);
				allocator.construct(gap_begin++, GAP_FORWARD(args));
			}
			else {
				size_type sz = m_next_size();
				gap_begin    = itr;
				gap_end      = gap_begin;
				pointer buf  = allocator.allocate(sz);
				try {
					m_move_to_newbuf(buf, sz);
				}
				catch (...) {
					allocator.deallocate(buf, sz);
					throw;
				}
				allocator.construct(gap_begin++, GAP_FORWARD(args));
			}
			return iterator(gap_begin - 1, gap_begin, gap_end);
		}

		template <typename InputIter>
		void m_range_insert(iterator itr, InputIter first, InputIter last, std::input_iterator_tag) {
			for (; first != last; ++first) {
				itr = insert(itr, *first);
				++itr;
			}
		}

		template <typename ForwardIter>
		void m_range_insert(const iterator& itr, ForwardIter first, ForwardIter last, std::forward_iterator_tag) {
			size_type sz = std::distance(first, last); // forward_iteratorの場合、若干非効率
			if (sz == 0)
				return;
			if (capacity() >= sz) {
				m_make_gap(itr);
				gap_begin = uninitialized_copy_a(first, last, gap_begin, allocator);
			}
			else {
				size_type nsz = std::max(size() + sz, m_next_size());
				pointer buf   = allocator.allocate(nsz);
				try {
					m_move_to_newbuf(buf, static_cast<pointer>(itr), nsz);
				}
				catch (...) {
					allocator.deallocate(buf, nsz);
					throw;
				}
				gap_begin = uninitialized_copy_a(first, last, gap_begin, allocator);
			}
		}


#if __GAP_CXX11__
		template <typename... Args>
		void m_emplace_back(Args&&... args)
#else
		void m_emplace_back(const value_type& args)
#endif
		{
			if (gap_end != gap_begin && gap_end == bend)
				allocator.construct(gap_begin++, GAP_FORWARD(args));
			else
				m_emplace(end(), GAP_FORWARD(args));
		}

#if __GAP_CXX11__
		template <typename... Args>
		void m_emplace_front(Args&&... args)
#else
		void m_emplace_front(const value_type& args)
#endif
		{
			if (gap_end != gap_begin && gap_begin == bbegin)
				allocator.construct(--gap_end, GAP_FORWARD(args));
			else
				m_emplace(begin(), GAP_FORWARD(args));
		}
	};

#undef GAP_MAKE_MOVE_ITER
#undef GAP_FORWARD

	template <typename Tp, typename Alloc>
	bool operator==(const gap_vector<Tp, Alloc>& lhs, const gap_vector<Tp, Alloc>& rhs) {
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	template <typename Tp, typename Alloc>
	bool operator!=(const gap_vector<Tp, Alloc>& lhs, const gap_vector<Tp, Alloc>& rhs) {
		return !(lhs == rhs);
	}

	template <typename Tp, typename Alloc>
	bool operator<(const gap_vector<Tp, Alloc>& lhs, const gap_vector<Tp, Alloc>& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <typename Tp, typename Alloc>
	bool operator>(const gap_vector<Tp, Alloc>& lhs, const gap_vector<Tp, Alloc>& rhs) {
		return rhs < lhs;
	}

	template <typename Tp, typename Alloc>
	bool operator<=(const gap_vector<Tp, Alloc>& lhs, const gap_vector<Tp, Alloc>& rhs) {
		return !(rhs < lhs);
	}

	template <typename Tp, typename Alloc>
	bool operator>=(const gap_vector<Tp, Alloc>& lhs, const gap_vector<Tp, Alloc>& rhs) {
		return !(lhs < rhs);
	}

	// gap_vectorをswapするときは、
	// ...
	// using std::swap;
	// gap::gap_vector<Tp,Alloc> gap1, gap2;
	// swap( gap1, gap2 );
	// ...
	// とすること
	// swapは例外を送出しない
	template <typename Tp, typename Alloc>
	void swap(gap_vector<Tp, Alloc>& lhs, gap_vector<Tp, Alloc>& rhs) {
		lhs.swap(rhs);
	}


} // namespace gap

#endif
