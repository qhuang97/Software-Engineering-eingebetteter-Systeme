#include "TestHelper.h"

using namespace std;

void print(string s)
{
    cout << "[" << sc_time_stamp() << " / " << sc_delta_count()
         << "](TestBench): " << s << endl;
}

void assert_at_time(sc_time expected_time, sc_fifo_in<bool> *port, string port_name)
{
    if (!port->read() || sc_time_stamp() != expected_time){
        print("Assertion failed: '" + port_name + "' is not true at time '" + expected_time.to_string() + "'");
        exit(1);
    }
}