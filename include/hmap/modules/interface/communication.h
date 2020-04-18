#ifndef HMAP_MODULES_INTERFACE_COMMUNICATION_H
#define HMAP_MODULES_INTERFACE_COMMUNICATION_H

#include <assert.h>

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

namespace fixtures {
class FCommunicator {
public:
    virtual int timeout() = 0;
    /**
     * To communicators capable of talking with each other
     */
    virtual void pair(Communicator*& c1, Communicator*& c2) = 0;
    void test() {
        Communicator* com1;
        Communicator* com2;
        this->pair(com1, com2); 
        
        const char* msg = "h";
        com1->send(msg, 1, 0);

        char* rcvd = nullptr;
        assert(com2->recv(rcvd, this->timeout()) == 1);
        assert(rcvd != nullptr);
        assert(rcvd[0] == 'h');
        

        // clean up
        delete com1;
        delete com2;
    }
};
} // fixtures

} // interface
} // hmap


#endif // HMAP_MODULES_INTERFACE_COMMUNICATION_H
