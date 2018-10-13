/*
 * static_queue.h
 *
 *  Created on: Sep 26, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_CIRCULAR_BUFFER_H_
#define SRC_BASE_CIRCULAR_BUFFER_H_

#include <cstddef>      // std::size_t
#include <stdint.h>     // uint8_t
#include <new>          // std::no_throw

class Circular_buffer {

public:
    Circular_buffer() {
        buffer_ = nullptr;

        head_ = 0;
        tail_ = 0;
        count_ = 0;
        buffer_size_ = 0;
        high_watermark_ = 0;
    }

    bool initialize(const std::size_t size) {
        bool return_val = false;

        // Check buffer doesn't already exist and that the requested size is valid
        if(nullptr == buffer_ && size >= 2) {

            buffer_ = new (std::nothrow) uint8_t[size];

            if(buffer_) {
                // buffer_ allocated; initialize member variables and indicate success to return
                buffer_size_ = size;

                head_ = 0;
                tail_ = buffer_size_-1;

                return_val = true;    // Buffer was created and class is initialized
            }
        }

        return (return_val);   // Any error occured
    }


    bool enqueue(const uint8_t& byte) {
        bool return_value = false;

        if(count_ < buffer_size_) {

            tail_ = (tail_+1) % buffer_size_;
            buffer_[tail_] = byte;
            count_ ++;

            if(count_ > high_watermark_) {
                high_watermark_ = count_;
            }

            return_value = true;
        }

        return (return_value);
    }


    bool dequeue(uint8_t& byte) {
        bool return_val = false;

        if(count_ > 0) {
            byte = buffer_[head_];
            head_ = (head_+1) % buffer_size_;
            count_--;

            return_val = true;
        }

        return(return_val);
    }


    const std::size_t get_free() {
        return(buffer_size_ - count_);
    }

    const std::size_t max_size() {
        return(buffer_size_);
    }

    const std::size_t high_watermark() {
        return(high_watermark_);
    }


private:
    uint8_t *buffer_;

    std::size_t head_;                   // Index where the next element will be removed from the buffer
    std::size_t tail_;                   // Index prior to where the next element will be inserted
    std::size_t count_;                  // Number of valid elements in the buffer
    std::size_t buffer_size_;            // How many entries are in T *buffer in total
    std::size_t high_watermark_;         // Maximum value count has reached

};  // class Circular_buffer



#endif /* SRC_BASE_CIRCULAR_BUFFER_H_ */
