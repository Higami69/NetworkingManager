#pragma once

#include <queue>
#include <mutex>

template <class T>
class SafeQueue
{
public:
	SafeQueue();
	~SafeQueue();

	SafeQueue(const SafeQueue& other) = delete;
	SafeQueue(SafeQueue&& other) = delete;
	SafeQueue operator=(const SafeQueue& other) = delete;
	SafeQueue operator=(SafeQueue&& other) = delete;

	void Push(T element);
	T Pop();
	bool IsEmpty() const;

private:
	std::queue<T> m_Queue;
	mutable std::mutex m_Mutex;
};

template <class T>
SafeQueue<T>::SafeQueue()
	:m_Queue()
	,m_Mutex()
{
}

template <class T>
SafeQueue<T>::~SafeQueue()
{
	while(!m_Queue.empty())
	{
		m_Queue.pop();
	}
}

template <class T>
void SafeQueue<T>::Push(T element)
{
	m_Mutex.lock();
	m_Queue.push(element);
	m_Mutex.unlock();
}

template <class T>
T SafeQueue<T>::Pop()
{
	m_Mutex.lock();
	T result{};
	if(!m_Queue.empty())
	{
		result = m_Queue.front();
		m_Queue.pop();
	}
	m_Mutex.unlock();

	return result;
}

template <class T>
bool SafeQueue<T>::IsEmpty() const
{
	return m_Queue.empty();
}
