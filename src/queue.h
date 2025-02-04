#pragma once

#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <optional>

template <typename T> struct Queue {
  Queue(size_t max_size) : max_size_(max_size) { buffer_.reserve(max_size); }

  bool push(T value){
    const std::lock_guard<std::mutex> lock{mutex_};
    if (buffer_.size() == max_size_){
        return false;
    }
    buffer_.push_back(value);
    return true;
  }

  std::optional<T> try_pop(){
    const std::lock_guard<std::mutex> lock{mutex_};
    if (buffer_.empty()){
        return {};
    }
    T ret = buffer_.pop_back();
    return ret;
  }

  std::vector<T> get_all() {
    const std::lock_guard<std::mutex> lock{mutex_};
    std::vector<T> ret = std::move(buffer_);
    return ret;
  }

private:
  std::vector<T> buffer_;
  size_t max_size_;
  std::mutex mutex_;
};

// TODO: move this to separate file
template<typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(size_t capacity)
        : m_buffer(capacity), m_capacity(capacity), m_head(0), m_tail(0), m_full(false)
    {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be greater than 0.");
        }
    }

    void push(const T& value) {
        m_buffer[m_head] = value;
        m_head = (m_head + 1) % m_capacity;

        if (m_full) {
            // If the buffer was already full, move tail forward
            m_tail = (m_tail + 1) % m_capacity;
        }

        // Check if full now
        m_full = (m_head == m_tail);
    }

    // Returns all elements in the buffer from oldest to newest
    std::vector<T> getAll() const {
        std::vector<T> elements;
        if (empty()) {
            return elements; // empty vector
        }

        if (m_full) {
            // The buffer is full, so the order starts at tail and goes to tail-1 (wrapped)
            for (size_t i = 0; i < m_capacity; ++i) {
                size_t idx = (m_tail + i) % m_capacity;
                elements.push_back(m_buffer[idx]);
            }
        } else {
            // The buffer is not full, so it spans from tail to head-1
            for (size_t i = m_tail; i != m_head; i = (i + 1) % m_capacity) {
                elements.push_back(m_buffer[i]);
            }
        }
        return elements;
    }

private:
    bool empty() const {
        return (!m_full && m_head == m_tail);
    }

    std::vector<T> m_buffer;
    const size_t m_capacity;
    size_t m_head;
    size_t m_tail;
    bool m_full;
};
