#pragma once

#include <initializer_list>
#include <array>
#include <cassert>

template <typename T, size_t N>
class FixedBuffer
{
public:
	FixedBuffer() = default;

	FixedBuffer(std::initializer_list<T> lst) 
		: m_Data {lst}
	{
	}

	template <typename V>
	void Add(V&& val)
	{
		m_Data[m_Index++] = std::forward<V>(val);
	}

	constexpr size_t Capacity() const { return N; }

	size_t Size() const { return m_Index; }

	void Clear() { m_Index = 0; }

	const T* Data() const { return m_Data.Data(); }
	T* Data() { return m_Data.Data(); }

	T& operator[](size_t index)
	{
		assert(index < m_Index);
		return m_Data[index];
	}

	const T& operator[](size_t index) const
	{
		assert(index < m_Index);
		return m_Data[index];
	}

	T* begin() { return m_Data.data(); }
	T* end() { return m_Data.data() + m_Index; }

	const T* begin() const { return m_Data.data(); }
	const T* end() const { return m_Data.data() + m_Index; }

private:
	std::array<T, N> m_Data;
	size_t m_Index = 0;
};