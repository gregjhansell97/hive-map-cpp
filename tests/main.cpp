
#include <iostream>
#include <unistd.h>
#include <memory>

//#include <hmap/testing.h>
#include <hmap/interface.h>
#include <hmap/simulation.h>


using std::cout;
using std::endl;


//using hmap::fixtures::FCommunicator;
using hmap::sim::LocalCommunicator;


int main() {
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
