#include "systemc.h"
#include "TestBench.h"
#include <string>

#ifndef Duengungssystem_H_
#define Duengungssystem_H_

extern bool stark_dun;

SC_MODULE(Duengungssystem)
{
    sc_in<bool> regen;

    sc_fifo_out<bool> salatfeld_duengen;
    sc_fifo_out<bool> kuerbisfeld_duengen;
    sc_fifo_out<bool> gewaechshaus_duengen;

    SC_CTOR(Duengungssystem)
    {

        SC_THREAD(sa_duengen);
        sensitive << regen;
        SC_THREAD(ku_duengen);
        sensitive << regen;
        SC_THREAD(ge_duengen);
        sensitive << regen;
    }

    void sa_duengen()
    {
        while (true)
        {
            wait(1500, SC_SEC);
            salatfeld_duengen.nb_write(true);
            wait(SC_ZERO_TIME);
        }
    }

    void ku_duengen()
    {
        while (true)
        {
            wait(SC_ZERO_TIME);
            while (true)
            {
                if (regen.read() == true)
                {
                    wait(600, SC_SEC);
                    kuerbisfeld_duengen.nb_write(true);

                    wait(300, SC_SEC);
                    kuerbisfeld_duengen.nb_write(true);

                    wait(10, SC_SEC);
                    if (regen.read() == false)
                    {
                        wait(770, SC_SEC);
                        kuerbisfeld_duengen.nb_write(true);
                    }
                }
                else if (regen.read() == false)
                {
                    while (true)
                    {
                        wait(1, SC_SEC);
                        if (regen.read() == true)
                        {
                            wait(599100, SC_MS);
                            kuerbisfeld_duengen.nb_write(true);
                            while (true)
                            {
                                wait(300, SC_SEC);
                                kuerbisfeld_duengen.nb_write(true);
                            }
                        }
                        wait(779, SC_SEC);
                        kuerbisfeld_duengen.nb_write(true);
                        wait(SC_ZERO_TIME);
                        if (regen.read() == 1)
                        {
                            wait(540100, SC_MS);
                            stark_dun = true;
                            while (stark_dun = true)
                            {
                                kuerbisfeld_duengen.nb_write(true);
                                wait(300, SC_SEC);
                                kuerbisfeld_duengen.nb_write(true);
                                wait(300, SC_SEC);
                                kuerbisfeld_duengen.nb_write(true);
                                wait(300, SC_SEC);
                                kuerbisfeld_duengen.nb_write(true);
                            }
                        }
                    }
                }
            }
        }
    }

    void ge_duengen()
    {
        while (true)
        {
            wait(1020, SC_SEC);
            gewaechshaus_duengen.nb_write(true);
            wait(SC_ZERO_TIME);
        }
    }
};

#endif