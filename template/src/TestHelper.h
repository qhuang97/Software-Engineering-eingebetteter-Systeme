#ifndef TESTHELPER_H_
#define TESTHELPER_H_

#include "systemc.h"

using namespace std;

#define SEQ_WAIT 0 // Time to wait between subsequent raise statements

void print(string s);

template <typename Type1>
void send_signal(sc_out<Type1> *port, string port_name, Type1 value)
{
    print("Setting '" + port_name + "'='" + std::to_string(value) + "'");
    port->write(value);
    wait(SEQ_WAIT, SC_MS);
}

//template <typename Type1>
void assert_at_time(sc_time expected_time, sc_fifo_in<bool> *port, string port_name);

template <typename Type2>
void assert_now(sc_in<Type2> *port, string port_name, Type2 expected, string message = "")
{
    // Check if signal value matches currently
    if (port->read() != expected)
    {
        print("Assertion failed: '" + port_name + "' is not '" + std::to_string(expected) + "'");
        exit(1);
    }
}

template <typename Type5>
void assert_100MS(sc_in<Type5> *port, string port_name, Type5 expected, string message = "")
{
    
    wait(100, SC_MS);
    if (port->read() != expected)
    {
        print("Assertion failed: '" + port_name + "' is not '" + std::to_string(expected) + "'");
        exit(1);
    }
}

template <typename Type3>
void send_signal_fifo(sc_fifo_out<Type3> *port, string port_name, Type3 value)
{
    print("Setting '" + port_name + "'='" + std::to_string(value) + "'");
    if (port->num_free() == 0){
        //buffer is full, this means you did not read the last signal yet. 
        //Your system has to read every signal! 
        print("Waring: The buffer of " + port_name + " is full! You have to read the signal or the test gets stuck here.");
    }
    port->write(value);
    wait(SEQ_WAIT, SC_MS);
}

template <typename Type4>
void assert_now(sc_fifo_in<Type4> *port, string port_name, Type4 expected, string message = "")
{
    // Check if signal value matches currently
    bool val;
    wait(SC_ZERO_TIME);
    if (port->nb_read(val) != expected)
    {
        print("Assertion failed: '" + port_name + "' is not '" + std::to_string(expected) + "'");
        exit(1);
    }
}


#endif