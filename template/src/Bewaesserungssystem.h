#include "systemc.h"
#include "TestBench.h"
#include <string>

#ifndef Bewaesserungssystem_H_
#define Bewaesserungssystem_H_

int fuellstand;

SC_MODULE(Bewaesserungssystem)
{
    sc_in<int> temperatur;
    sc_in<bool> regen;
    sc_fifo_out<bool> salatfeld_bewaessern;
    sc_fifo_out<bool> kuerbisfeld_bewaessern;
    sc_fifo_out<bool> gewaechshaus_bewaessern;

    int add;

    sc_fifo_in<int> wassertank_auffuellen;
    sc_out<bool> wassertank_warnlampe;

    sc_event check1;
    sc_event check2;
    sc_event check3;

    SC_CTOR(Bewaesserungssystem)
    {

        SC_THREAD(sa_bewaessern);
        sensitive << regen << temperatur;
        SC_THREAD(ku_bewaessern);
        sensitive << regen << temperatur;
        SC_THREAD(ge_bewaessern);
        sensitive << regen << temperatur;

        fuellstand = 0;
        add = 0;
        wassertank_warnlampe.initialize(true);
        SC_THREAD(auffuellen);
        sensitive << wassertank_auffuellen;
        SC_THREAD(lampe);
        sensitive << regen << wassertank_auffuellen;
        SC_THREAD(auffuellenmitregen);
        sensitive << regen << wassertank_auffuellen;
    }

    void sa_bewaessern()
    {
        while (true)
        {
            wait(660, SC_SEC, regen.default_event());
            if (regen.read())
            {
                wait();
            }
            else
            {
                salatfeld_bewaessern.nb_write(true);
                wait(SC_ZERO_TIME);
            }
        }
    }
    void ku_bewaessern()
    {
        while (true)
        {
            if (temperatur.read() <= 30)
            {
                wait(480, SC_SEC, regen.default_event());
                if (regen.read())
                {
                    wait();
                }
                else
                {
                    kuerbisfeld_bewaessern.nb_write(true);
                    wait(SC_ZERO_TIME);
                }
            }
            else
            {
                wait(360, SC_SEC, regen.default_event());
                if (regen.read())
                {
                    wait();
                }
                else
                {
                    kuerbisfeld_bewaessern.nb_write(true);
                    wait(SC_ZERO_TIME);
                }
            }
        }
    }
    void ge_bewaessern()
    {
        while (true)
        {
            wait(95, SC_SEC);
            gewaechshaus_bewaessern.nb_write(true);
            fuellstand -= 1000;
            check3.notify();
            wait(SC_ZERO_TIME);
        }
    }
    void lampe()
    {
        while (true)
        {
            wait(check1 | check2 | check3);
            if (fuellstand < 1000)
            {
                wassertank_warnlampe.write(true);
                wait(SC_ZERO_TIME);
            }
            else
            {
                wassertank_warnlampe.write(false);
                wait(SC_ZERO_TIME);
            }
        }
    }

    void auffuellen()
    {
        while (true)
        {
            add = wassertank_auffuellen.read();
            if ((add + fuellstand) >= 5000)
            {
                fuellstand = 5000;
                check1.notify();
            }
            else
            {
                fuellstand += add;
                check1.notify();
                wait(SC_ZERO_TIME);
            }
        }
    }

    void auffuellenmitregen()
    {
        while (true)
        {
            if (regen.read() == false)
            {
                wait(95, SC_SEC, regen.default_event());
            }
            else if (regen.read() == true)
            {
                wait(95, SC_SEC, regen.default_event());
                if (regen.read() == true)
                {
                    if (100 + fuellstand >= 5000)
                    {
                        fuellstand = 5000;
                        check2.notify();
                    }
                    else
                    {
                        fuellstand += 100;
                        check2.notify();
                    }
                }
                else if (regen.read() == false)
                {
                    fuellstand += 0;
                    check2.notify();
                }
            }
        }
    }
};

#endif