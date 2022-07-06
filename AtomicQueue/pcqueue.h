// Copyright (c) 2020 MIT Digital Currency Initiative, 
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PCQUEUE_H
#define PCQUEUE_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <atomic>
#include <string>
#include <array>

template<typename T, size_t capacity>
class AtomicQueue {
    public:
        bool push(const T& item) {
            const auto r = m_read_pos.load(std::memory_order_consume);
            const auto w = m_write_pos.load(std::memory_order_consume);

            const auto next_w = forward(w);

            if(r == next_w) {
                return false;
            }

            m_buffer[w] = std::move(item);
            m_write_pos.store(next_w, std::memory_order_release);

            return true;
        }

        bool pop(T& item) {
            const auto r = m_read_pos.load(std::memory_order_consume);
            const auto w = m_write_pos.load(std::memory_order_consume);

            if(r == w) {
                return false;
            }

            item = std::move(m_buffer[r]);
            m_read_pos.store(forward(r), std::memory_order_release);

            return true;
        }

        void clear() {
            m_write_pos = 0;
            m_read_pos = 0;
        }

    private:
        std::atomic<size_t> m_read_pos{0};
        std::atomic<size_t> m_write_pos{0};
        std::array<T, capacity + 1> m_buffer;

        size_t forward(size_t pos) {
            return (pos + 1) % m_buffer.size();
        }
};

template <class T, size_t capacity>
class PCQueue
{
public:
    void add(T item) {
        while(!m_queue.push(item)) {
            m_cond.notify_all();
        }
        m_cond.notify_all();
    }

    void run(std::function<void(std::vector<T>)> func) {
        while(true)
        {
            std::vector<T> items;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cond.wait(lock);
                if(m_interrupt) {
                    break;
                }
            }

            T val;
            while(m_queue.pop(val)) {
                items.push_back(std::move(val));
            }
            func(std::move(items));
        }
    }

    void interrupt() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_interrupt = true;
        }
        m_cond.notify_all();
    }

    void shutdown() {
        m_queue.clear();
        interrupt();
    }

    ~PCQueue() {
        shutdown();
    }

private:
    AtomicQueue<T, capacity> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_interrupt{false};
};

#endif // PEERQUEUE_H
