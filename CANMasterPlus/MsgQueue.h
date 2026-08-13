#pragma once
#include <queue>
#include <mutex>
#include <thread>

template<typename T>
class MsgQueue
{
public:
	MsgQueue() = default;
	~MsgQueue() = default;

	MsgQueue(const MsgQueue&) = delete;
	MsgQueue& operator=(const MsgQueue&) = delete;
	void push_back(const T& value)
	{
		std::lock_guard<std::mutex>lock(this->m_mutex);
		//only save latest 2 values
		while (this->m_queue.size() > this->m_size-1)
		{
			this->m_queue.pop();
		}
		this->m_queue.push(value);

	}
	bool pop(T& res)
	{
		std::lock_guard<std::mutex>lock(this->m_mutex);
		if (this->m_queue.size() <= 0)
			return false;
		res = this->m_queue.front();
		this->m_queue.pop();
		return true;
	}
private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	int8_t m_size=2;
};

