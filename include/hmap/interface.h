#ifndef HMAP_INTERFACE_H
#define HMAP_INTERFACE_H

#include <hmap/modules/interface/communication.h>

#if 0
namespace hmap {
namespace interface {

class Communicator {
public:
    static const ssize_t error = -1;
    static const ssize_t closed = -2;
    /**
     * Sends data to one or more communicators
     *
     * Args:
     *     data: the raw-bytes of data being sent
     *     size: number of bytes of the data being sent
     *     timeout: if send is blocking, how long should it wait to send
     *
     * Returns:
     *     status value regarding the success of sending
     */
    virtual ssize_t send(
            const char* data, const size_t size, const int timeout) = 0;

    /**
     * Receives bytes of data from another communicator
     *
     * Args: 
     *     data: raw-bytes received (passed-by-reference)
     *     size: number of bytes received (passed-by-reference)
     *     timeout: blocks on receive
     *
     * Returns:
     *     status value regarding success of sending
     */
    virtual ssize_t recv(char*& data, const int timeout) = 0;

    /**
     * Idempotent method that ends send and recv capabilities
     */
    virtual void close() = 0;
};

} // interface
} // hmap
#endif


#endif // HMAP_INTERFACE
