#include "systemc.h"
#include "TestBench.h"
#include <string>

#ifndef Pflanzenschutzsystem_H_
#define Pflanzenschutzsystem_H_

SC_MODULE(Pflanzenschutzsystem)
{

    sc_fifo_in<bool> salatfeld_blattlaeuse;
    sc_fifo_in<bool> salatfeld_schnecken;
    // KÜRBISFELD
    sc_fifo_in<bool> kuerbisfeld_blattlaeuse;

    sc_fifo_out<bool> kuerbisfeld_pflanzenschutz;
    sc_fifo_out<bool> salatfeld_pflanzenschutz;

    SC_CTOR(Pflanzenschutzsystem)
    {

        SC_THREAD(sab_schutz);
        SC_THREAD(sas_schutz);
        SC_THREAD(ku_schutz);
    }

    void sab_schutz()
    {

        while (true)
        {

            if (salatfeld_blattlaeuse.read())
            {
                salatfeld_pflanzenschutz.nb_write(true);
            }
        }
    }

    void sas_schutz()
    {

        while (true)
        {

            if (salatfeld_schnecken.read())
            {
                salatfeld_pflanzenschutz.nb_write(true);
                wait(20, SC_SEC);
                salatfeld_pflanzenschutz.nb_write(true);
                wait(1, SC_SEC);
            }
        }
    }

    void ku_schutz()
    {

        while (true)
        {

            if (kuerbisfeld_blattlaeuse.read())
            {

                kuerbisfeld_pflanzenschutz.nb_write(true);
            }
        }
    }
};

#endif