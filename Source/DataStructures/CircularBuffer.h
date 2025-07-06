#pragma once

#include <array>
#include <iterator>
#include <cstddef>
#include <cassert>

template <typename T, size_t N>
class CircularBuffer
{
public:
	CircularBuffer() = default;

	CircularBuffer(std::initializer_list<T> lst) :
		m_Tail{ lst.size() % N }
	{
		assert(lst.size() <= N);
		std::copy(lst.begin(), lst.end(), m_Data.begin());
	}

	size_t Size() const {
		if (m_Tail >= m_Head)
		{
			return m_Tail - m_Head;
		}
		else
		{
			return N - m_Head + m_Tail;
		}
	}

	template <typename V>
	void Insert(V&& val)
	{
		m_Data[m_Tail] = std::forward<V>(val);
		m_Tail = (m_Tail + 1) % N;
		assert(m_Tail != m_Head);
	}

	void Pop()
	{
		assert(m_Head != m_Tail);
		m_Head = (m_Head + 1) % N;
	}

	T& Top()
	{
		assert(Size() > 0);
		return m_Data[m_Head];
	}

	const T& Top() const
	{
		assert(Size() > 0);
		return m_Data[m_Head];
	}

	template <bool IsConst>
	class Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = std::conditional_t<IsConst, const T*, T*>;
		using reference = std::conditional_t<IsConst, const T&, T&>;

		Iterator(pointer ptr, size_t index) : m_Ptr{ ptr }, m_Index{ index } {}

		reference operator*() const { return m_Ptr[m_Index]; }

		pointer operator->() const { return m_Ptr + m_Index; }

		Iterator& operator++() { m_Index = (m_Index + 1) % N; return *this; }

		bool operator==(const Iterator& other) const { return m_Ptr == other.m_Ptr && m_Index == other.m_Index; }
		bool operator!=(const Iterator& other) const { return m_Ptr != other.m_Ptr || m_Index != other.m_Index; }

	private:
		pointer m_Ptr;
		size_t m_Index;
	};

	using iterator = Iterator<false>;
	using const_iterator = Iterator<true>;

	iterator begin() { return iterator { m_Data.data(), m_Head }; }
	iterator end() { return iterator { m_Data.data(), m_Tail }; }

	const_iterator begin() const { return const_iterator { m_Data.data(), m_Head }; }
	const_iterator end() const { return const_iterator { m_Data.data(), m_Tail }; }

private:
	size_t m_Head = 0;
	size_t m_Tail = 0;
	std::array<T, N> m_Data;
};
