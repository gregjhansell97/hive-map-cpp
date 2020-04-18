
#include <iostream>
#include <unistd.h>
#include <memory>

//#include <hmap/testing.h>
#include <hmap/interface.h>
#include <hmap/simulation.h>


using std::cout;
using std::endl;


using hmap::interface::fixtures::FCommunicator;
using hmap::interface::Communicator;
using hmap::sim::LocalCommunicator;


class FLocalCommunicator: public FCommunicator {
    int timeout() { return 100; }
    void pair(Communicator*& c1, Communicator*& c2) {
        LocalCommunicator* lc1 = new LocalCommunicator();
        LocalCommunicator* lc2 = new LocalCommunicator();
        lc1->connect(*lc2);
        lc2->connect(*lc1);
        c1 = lc1;
        c2 = lc2;
    }
};


int main() {
    FLocalCommunicator fixture;
    fixture.test();

    LocalCommunicator c1;
    LocalCommunicator c2;
    c1.connect(c2);
    c2.connect(c1);
    c1.send("Hello word\0", 11, 0);
    
    char* msg;
    cout << c2.recv(msg, 1) << endl;
    cout << msg << endl;

    cout << "msg: " <<  c2.recv(msg, 1000) << endl;


    c2.send("GOOD BYE\0", 9, 0);
    
    cout << c1.recv(msg, 1) << endl;
    cout << msg << endl;  

    
    return 0;
}
