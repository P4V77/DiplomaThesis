#pragma once

#include <cstdint>
#include <atomic>



/*
 * Inspired by: https://embeddedartistry.com/blog/2017/4/6/circular-buffers-in-cc
 *
 */

namespace nd::utils
{
    template<typename T, const uint32_t SIZE>
    class Fifo
    {
        private:
            T queue[SIZE];
            std::atomic<uint32_t> read_position;
            std::atomic<uint32_t> write_position;

        public:
            Fifo();

            void write(const T& data);
            [[nodiscard]] T read();
            [[nodiscard]] bool is_empty() const;
            [[nodiscard]] bool is_full() const;
    };

    template<typename T, const uint32_t SIZE>
    Fifo<T, SIZE>::Fifo(): read_position(0),
                           write_position(0)
    {}

    template<typename T, const uint32_t SIZE>
    void Fifo<T, SIZE>::write(const T& data)
    {
        queue[write_position] = data;

        if (is_full())
        {
            read_position = (read_position + 1) % SIZE;
        }

        write_position = (write_position + 1) % SIZE;
    }

    template<typename T, const uint32_t SIZE>
    T Fifo<T, SIZE>::read()
    {
        const auto value = queue[read_position];

        read_position = (read_position + 1) % SIZE;

        return value;
    }

    template<typename T, const uint32_t SIZE>
    bool Fifo<T, SIZE>::is_empty() const
    {
        return (write_position == read_position);
    }

    template<typename T, const uint32_t SIZE>
    bool Fifo<T, SIZE>::is_full() const
    {
        return (((write_position + 1) % SIZE) == read_position);
    }
}
