#ifndef HMAP_SIMULATION
#define HMAP_SIMULATION

#include <mutex>
#include <vector>
#include <queue>

#include <hmap/interface.h>

// public in heritance
namespace hmap {
namespace sim {

class LocalCommunicator : hmap::interface::Communicator {
public:
    LocalCommunicator();
    ~LocalCommunicator();
    ssize_t send(
            const char* data, 
            const size_t size, 
            const int timeout) override;
    ssize_t recv(char*& data, const int timeout) override;
    void close() override;

    void connect(LocalCommunicator& connection);
    void disconnect(LocalCommunicator& disconnect);

private:
    struct Msg {
        char* data;
        size_t size;
    };
    void deliver(LocalCommunicator::Msg msg);

    std::mutex m_mailbox_mtx;
    std::mutex m_neighbors_mtx;

    std::vector<LocalCommunicator*> m_neighbors;
    std::queue<LocalCommunicator::Msg> m_mailbox;

    int m_close_trigger;
    int m_close_flag;
    int m_mailbox_trigger;
    int m_mailbox_flag;
};

} // sim
} // hmap

#endif
