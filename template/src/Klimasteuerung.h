#include "systemc.h"
#include "TestBench.h"
#include <string>

#ifndef Klimasteuerung_H_
#define Klimasteuerung_H_

SC_MODULE(Klimasteuerung)
{
    sc_in<int> temperatur;
    sc_in<bool> regen;
    sc_in<TestBench::WetterTyp> wetter;

    sc_out<int> gewaechshaus_heizung;

    SC_CTOR(Klimasteuerung)
    {

        gewaechshaus_heizung.initialize(1);
        SC_THREAD(heizung);
        sensitive << temperatur << wetter << regen;
    }

    void heizung()
    {

        while (true)
        {

            wait();

            if (temperatur.read() > 30)
            {

                if (wetter.read() == TestBench::WetterTyp::sonnig)
                {

                    gewaechshaus_heizung.write(0);
                }
                if (wetter.read() == TestBench::WetterTyp::bewoelkt)
                {

                    gewaechshaus_heizung.write(0);
                }
                if (regen.read() == true)
                {

                    gewaechshaus_heizung.write(1);
                }
                if (wetter.read() == TestBench::WetterTyp::stark_bewoelkt)
                {

                    gewaechshaus_heizung.write(1);
                }
                if (wetter.read() == TestBench::WetterTyp::dunkel)
                {

                    gewaechshaus_heizung.write(2);
                }
            }
            if (temperatur.read() <= 30)
            {

                if (wetter.read() == TestBench::WetterTyp::sonnig)
                {

                    gewaechshaus_heizung.write(1);
                }
                if (wetter.read() == TestBench::WetterTyp::bewoelkt)
                {

                    gewaechshaus_heizung.write(2);
                }
                if (wetter.read() == TestBench::WetterTyp::stark_bewoelkt)
                {

                    gewaechshaus_heizung.write(3);
                }
                if (regen.read() == true)
                {

                    gewaechshaus_heizung.write(3);
                }
                if (wetter.read() == TestBench::WetterTyp::dunkel)
                {

                    gewaechshaus_heizung.write(4);
                }
            }
        }
    }
};

#endif