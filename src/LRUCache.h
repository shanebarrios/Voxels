#pragma once

#include <cassert>
#include <unordered_map>

template <typename TKey, typename TVal>
class LRUCache
{
public:
	explicit LRUCache(size_t capacity) : m_Capacity{ capacity } 
	{
		assert(capacity > 0);
	};

	~LRUCache()
	{
		Node* cur = m_Head;
		while (cur)
		{
			Node* next = cur->Next;
			delete cur;
			cur = next;
		}
	}

	LRUCache(const LRUCache&) = delete;
	LRUCache& operator=(const LRUCache&) = delete;
	LRUCache(LRUCache&&) = delete;
	LRUCache& operator=(LRUCache&&) = delete;

	const TVal* Get(const TKey& key) 
	{
		if (auto it = m_Cache.find(key); it != m_Cache.end()) 
		{
			Node* const node = it->second;
			MoveToHead(node);
			return &node->Val;
		}
		return nullptr;
	}
	
	template <typename K, typename V>
	const TVal& Insert(K&& key, V&& val) 
	{
		if (auto it = m_Cache.find(key); it != m_Cache.end())
		{
			Node* const node = it->second;
			node->Val = std::forward<V>(val);
			MoveToHead(node);
			return node->Val;
		}

		Node* const newNode = new Node{ std::forward<K>(key), std::forward<V>(val), m_Head, nullptr };
		m_Cache[newNode->Key] = newNode;

		if (m_Head == nullptr)
		{
			m_Head = newNode;
			m_Tail = newNode;
			return newNode->Val;
		}

		m_Head = newNode;
		m_Head->Next->Prev = m_Head;
		
		if (m_Cache.size() > m_Capacity)
		{
			EvictTail();
		}
		return newNode->Val;
	}
private:
	struct Node 
	{
		TKey Key;
		TVal Val;
		Node* Next;
		Node* Prev;
	};

	size_t m_Capacity;
	Node* m_Head = nullptr;
	Node* m_Tail = nullptr;
	std::unordered_map<TKey, Node*> m_Cache{};

	void MoveToHead(Node* node)
	{
		if (node == m_Head) return;

		if (node == m_Tail) m_Tail = node->Prev;
		
		if (node->Next) node->Next->Prev = node->Prev;

		if (node->Prev) node->Prev->Next = node->Next;

		node->Next = m_Head;
		node->Prev = nullptr;
		m_Head->Prev = node;
		m_Head = node;
	}

	void EvictTail() 
	{
		auto it = m_Cache.find(m_Tail->Key);
		m_Cache.erase(it);
		Node* newTail = m_Tail->Prev;
		newTail->Next = nullptr;
		delete m_Tail;
		m_Tail = newTail;
	}
};