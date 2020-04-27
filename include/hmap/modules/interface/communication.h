#ifndef HMAP_MODULES_INTERFACE_COMMUNICATION_H
#define HMAP_MODULES_INTERFACE_COMMUNICATION_H

#include <assert.h>

namespace hmap {
namespace interface {

class Communicator {
public:
    static const ssize_t error = -1; // returned on error for send/recv
    static const ssize_t closed = -2; // returned if closed during send/recv
    /**
     * Sends data to one or more communicators
     *
     * Args:
     *     data: the raw-bytes of data being sent
     *     size: number of bytes of the data being sent
     *     timeout: if send is blocking, how long should it wait to send
     *
     * Returns:
     *     status value regarding the success of sending, if less than zero
     *     then either an error occurred (Communicator::error) or the
     *     communicator closed during the send operation
     */
    virtual ssize_t send(
            const char* data, const size_t size, const int timeout) = 0;

    /**
     * Receives bytes of data from another communicator
     *
     * Args: 
     *     data: raw-bytes received (pointer to a pointer), new memory is
     *      is allocated (must be deleted) TODO change to a shared_ptr
     *     size: number of bytes received (passed-by-reference)
     *     timeout: blocks on receive
     *
     * Returns:
     *     status value regarding success of sending. if zero then nothing was
     *     received. If less than zero then either an error occurred
     *     (Communicator::error) or the communicator closed during the recv
     *     operation
     */
    virtual ssize_t recv(char** data, const int timeout) = 0;

    /**
     * Idempotent method that ends send and recv capabilities and wraps up any
     * concurrency black magic going on
     */
    virtual void close() = 0;
};

namespace fixtures {
class FCommunicator {
public:
    /**
     * Tests basic send and receive amongst several communicators
     *
     * Args:
     *     connected: list of 2 or more communicators all able to directly
     *         communicate with one another
     *     isolated: list of 1 or more communicators that cannot communicate
     *         with the connected communicators
     */
    void test_send_recv(
            Communicator* connected, const size_t c_size,
            Communicator* isolated, const size_t i_size, 
            const int timeout=1) {
        const char* msg = "h";
        // sends message of size 1 with 0 timeout to all 
        connected[0]->send(msg, 1, 0);
        for(size_t i = 1; i < c_size; ++i) {
            char* rcvd = nullptr;
            auto c = connected[i];
            // receive data from a communicator in range
            assert(c->recv(rcvd, timeout));
            assert(rcvd != nullptr);
            assert(rcvd[0] == 'h');
            delete [] rcvd;
        }
        for(size_t i = 0; i < i_size; ++i) {
            char* rcvd = nullptr;
            assert(isolated[i]->recv(rcvd, 0) == 0);
            assert(rcvd == nullptr);
        }
    }
};
} // fixtures

} // interface
} // hmap


#endif // HMAP_MODULES_INTERFACE_COMMUNICATION_H
