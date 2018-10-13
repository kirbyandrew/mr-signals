/*
 * circular_buffer_tests.cpp
 *
 *  Created on: Sep 30, 2018
 *      Author: ackpu
 */


#include "gtest/gtest.h"
#include "circular_buffer.h"
#include <cstddef>
#include <iostream>


/*
 * Test all expecte behaviour for an instantiated but non-initialized Circular_buffer object
 */
TEST(CircularBuffer,Setup)
{
    Circular_buffer circular_buffer;

    uint8_t test_byte;

    // Should not be able to enqueue or deqeue as not initialized
    EXPECT_FALSE(circular_buffer.enqueue(test_byte));
    EXPECT_FALSE(circular_buffer.dequeue(test_byte));

    // Confirm buffer characteristics
    EXPECT_EQ(circular_buffer.max_size(),0U);
    EXPECT_EQ(circular_buffer.get_free(),0U);
    EXPECT_EQ(circular_buffer.high_watermark(),0U);


    // Attempt to create an invalid size
    EXPECT_FALSE(circular_buffer.initialize(1));


    // Check everything is still as expected
    EXPECT_EQ(circular_buffer.max_size(),0U);
    EXPECT_EQ(circular_buffer.get_free(),0U);
    EXPECT_EQ(circular_buffer.high_watermark(),0U);




    // Now set a valid size
    EXPECT_TRUE(circular_buffer.initialize(2));

    // Check everything is as expected
    EXPECT_EQ(circular_buffer.max_size(),2U);
    EXPECT_EQ(circular_buffer.get_free(),2U);
    EXPECT_EQ(circular_buffer.high_watermark(),0U);




    // Try Setting again with larger buffer; should fail and buffer should remain same size
    EXPECT_FALSE(circular_buffer.initialize(3));

    EXPECT_EQ(circular_buffer.max_size(),2U);
    EXPECT_EQ(circular_buffer.get_free(),2U);
    EXPECT_EQ(circular_buffer.high_watermark(),0U);
}

TEST(CircularBuffer,Filling)
{
    Circular_buffer circular_buffer;
    const std::size_t buffer_size = 4;



    EXPECT_TRUE(circular_buffer.initialize(buffer_size ));

    uint8_t byte_in=1, byte_out=2;


    // Enqueue one byte then dequeue it into another variable that is initialized to another value
    // and ensure that they match after the dequeueing
    EXPECT_TRUE(circular_buffer.enqueue(byte_in));


    EXPECT_EQ(circular_buffer.max_size(),buffer_size);
    EXPECT_EQ(circular_buffer.get_free(),buffer_size-1);    // Should be 1 less free
    EXPECT_EQ(circular_buffer.high_watermark(),1U);



    EXPECT_TRUE(circular_buffer.dequeue(byte_out));
    EXPECT_EQ(byte_in, byte_out);

    EXPECT_EQ(circular_buffer.max_size(),buffer_size);
    EXPECT_EQ(circular_buffer.get_free(),buffer_size);  // Back to completely free
    EXPECT_EQ(circular_buffer.high_watermark(),1U);     // High watermark should not go down



    // Now fill the buffer up
    for(std::size_t loop_count=1; loop_count < (buffer_size+1) ; loop_count++) {     // Loop over size of the buffer

        EXPECT_TRUE(circular_buffer.enqueue(loop_count+2));        // Enqueue some new values (3,4,5,6)

        EXPECT_EQ(circular_buffer.max_size(),buffer_size);
        EXPECT_EQ(circular_buffer.get_free(),buffer_size-loop_count);  // Free should reduce through this loop
        EXPECT_EQ(circular_buffer.high_watermark(),loop_count);        // High watermark should increase in this loop
    }

    // Should not be able to add anything new
    EXPECT_FALSE(circular_buffer.enqueue(byte_in));

    // Confirm expected characteristics (no free space)
    EXPECT_EQ(circular_buffer.max_size(),buffer_size);
    EXPECT_EQ(circular_buffer.get_free(),0U);
    EXPECT_EQ(circular_buffer.high_watermark(),buffer_size);


    // And empty it
    for(std::size_t loop_count=1; loop_count < (buffer_size+1); loop_count++) {     // Loop over size of the buffer

        uint8_t byte = 0xFF;

        EXPECT_TRUE(circular_buffer.dequeue(byte));

        EXPECT_EQ(byte, (uint8_t)loop_count+2);     // Compare to expected FIFO entered data (3,4,5,6)

        EXPECT_EQ(circular_buffer.max_size(),buffer_size);
        EXPECT_EQ(circular_buffer.get_free(),loop_count);           // Free should increase through this loop
        EXPECT_EQ(circular_buffer.high_watermark(),buffer_size);    // High watermark should stay at max
    }

}
