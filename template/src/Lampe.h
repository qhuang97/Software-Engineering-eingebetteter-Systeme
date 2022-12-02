#include "systemc.h"
#include "TestBench.h"
#include <string>

#ifndef Lampe_H_
#define Lampe_H_

SC_MODULE(Lampe)
{
    sc_in<int> temperatur;
    sc_in<bool> regen;
    sc_in<TestBench::WetterTyp> wetter;

    TestBench::WetterTyp w;

    sc_out<int> gewaechshaus_lampe;

    SC_CTOR(Lampe)
    {

        gewaechshaus_lampe.initialize(0);
        SC_THREAD(lampe);
        sensitive << temperatur << regen << wetter;
    }

    void lampe()
    {
        while (true)
        {

            wait();
            w = wetter.read();

            if (w == TestBench::WetterTyp::sonnig)
            {

                gewaechshaus_lampe.write(4);
                wait(SC_ZERO_TIME);
                if (regen.read())
                {
                    gewaechshaus_lampe.write(1);
                }
                else
                {
                    gewaechshaus_lampe.write(0);
                }
            }
            else if (w == TestBench::WetterTyp::bewoelkt)
            {
                gewaechshaus_lampe.write(0);
                wait(SC_ZERO_TIME);
                if (regen.read())
                {
                    gewaechshaus_lampe.write(1);
                }
            }

            else if (w == TestBench::WetterTyp::stark_bewoelkt)
            {
                gewaechshaus_lampe.write(2);
                wait(SC_ZERO_TIME);
                if (regen.read())
                {
                    gewaechshaus_lampe.write(3);
                }
            }
            else if (w == TestBench::WetterTyp::dunkel)
            {
                gewaechshaus_lampe.write(3);
                wait(SC_ZERO_TIME);
                if (regen.read())
                {
                    gewaechshaus_lampe.write(3);
                }
            }
        }
    }
};

#endif