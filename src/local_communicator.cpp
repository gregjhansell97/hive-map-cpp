// c-style imports 
#include <assert.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

// c++ std
#include <memory>

// hmap imports
#include <hmap/simulation.h>


namespace hmap {
namespace sim {

LocalCommunicator::LocalCommunicator() {
    // pipe for close trigger/flag
    int p[2];
    if (pipe(p) < 0) {
        exit(1);
    }
    // [read, write]
    m_close_trigger = p[1];
    m_close_flag = p[0];

    // pipe for msg trigger/flag
    if (pipe(p) < 0) {
        exit(1);
    }
    // [read, write]
    m_mailbox_trigger = p[1];
    m_mailbox_flag = p[0];
}

ssize_t LocalCommunicator::send(
        const char*  data, 
        const size_t size, 
        const int timeout) {
    // go through neighbors and deliver copy of message to each communicator
    pollfd closed_status[1] = {
        {m_close_flag, POLLIN, 0}
    };
    if(poll(closed_status, 1, 0)) { // communicator is done
        return Communicator::closed;
    } 
    m_neighbors_mtx.lock();
        for(auto c: m_neighbors) {
            if(c == nullptr) continue;
            char* c_data = new char(size);
            // destination is msg, source is data
            memcpy(c_data, data, size);
            // std::move moves the unique pointer 
            c->deliver({c_data, size});
        }
    m_neighbors_mtx.unlock();
    return 1; 
}

void LocalCommunicator::deliver(LocalCommunicator::Msg msg) {
    m_mailbox_mtx.lock();
        m_mailbox.push(msg);
    m_mailbox_mtx.unlock();
    write(m_mailbox_trigger, "g", 1); // new mail
}

ssize_t LocalCommunicator::recv(
        char*& data,
        const int timeout) {
    data = nullptr;
    pollfd flags[2] = {
        {m_close_flag, POLLIN, 0},
        {m_mailbox_flag, POLLIN, 0}
    };
    const int status = poll(flags, 2, timeout);
    if(status < 0) {
        // some error occurred
        // TODO will have to descern errno from status options returned
        return Communicator::error;
    } else if (status == 0) {
        // time-out happend
        return 0;
    } else if(poll(&flags[0], 1, 0)) { // communicator is done
        return Communicator::closed;
    } 
    m_mailbox_mtx.lock();
        if(poll(&flags[1], 1, 0)) { // received message 
                ssize_t size = 0;
                char m;
                read(m_mailbox_flag, &m, 1);
                // grab latest message from mailbox
                if(!m_mailbox.empty()) {
                    // message 
                    data = m_mailbox.front().data;
                    size = m_mailbox.front().size;
                    // remove it from mail
                    m_mailbox.pop();
                }
            m_mailbox_mtx.unlock();
            return size; // success
        }
    m_mailbox_mtx.unlock();
    // well fuck
    assert(false);
    return 0;
}

void LocalCommunicator::close() {
    pollfd closed_status[1] = {
        {m_close_flag, POLLIN, 0}
    };
    if(poll(closed_status, 1, 0)) {
        return; // communicator already done
    } else {
        m_neighbors_mtx.lock();
            write(m_close_trigger, "g", 1);
            for(size_t i = 0; i < m_neighbors.size(); ++i) {
                auto c = m_neighbors[i];
                if(c == nullptr) continue;
                this->disconnect(*c);
                m_neighbors[i] = nullptr;
            }
        m_neighbors_mtx.unlock();
    }
}

void LocalCommunicator::disconnect(LocalCommunicator& connection) {
    connection.m_neighbors_mtx.lock();
        for(size_t i = 0; i < connection.m_neighbors.size(); ++i) {
            auto c = connection.m_neighbors[i];
            if(c == this) {
                connection.m_neighbors[i] = nullptr;
            }
        }
    connection.m_neighbors_mtx.unlock();
}


void LocalCommunicator::connect(LocalCommunicator& connection) {
    pollfd closed_status[1] = {
        {m_close_flag, POLLIN, 0}
    };
    connection.m_neighbors_mtx.lock();
        // CHECK IF CLOSING
        if(poll(closed_status, 1, 0)) {
            connection.m_neighbors_mtx.unlock();
            return;
        }
        // see if there is a spot to put connection or already existing
        bool assigned = false;
        for(size_t i = 0; i < connection.m_neighbors.size(); ++i) {
            LocalCommunicator* c = connection.m_neighbors[i];
            if(c == nullptr) {
                // found a spot!
                connection.m_neighbors[i] = this;
                assigned = true;
            } else if(c == this) {
                // found previous assignment
                if(assigned) { // made a new assignment already (get rid of old)
                    connection.m_neighbors[i] = nullptr;
                }
                assigned = true;
                break;
            }
        }
        if(!assigned) {
            // did not find space or a pre-existing connection
            connection.m_neighbors.push_back(this);
        }
    connection.m_neighbors_mtx.unlock();
}

LocalCommunicator::~LocalCommunicator() {
    this->close(); //close if not already
    // close fds
    ::close(m_close_trigger);
    ::close(m_close_flag);
    ::close(m_mailbox_trigger);
    ::close(m_mailbox_flag);
    // free memory in mailbox
    while(!m_mailbox.empty()) {
        delete [] m_mailbox.front().data;
        m_mailbox.pop();
    }
}   



} //sim
} //hmap
