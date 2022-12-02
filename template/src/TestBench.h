#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "systemc.h"
#include <string>
#include <map>

using namespace std;

SC_MODULE(TestBench)
{
    enum WetterTyp
    {
        sonnig = 0,
        bewoelkt = 1,
        stark_bewoelkt = 2,
        dunkel = 3
    };

    // ------------------- Out Events -------------------
    
    //WETTER
    sc_out<bool> regen;
    sc_out<int> temperatur;
    sc_out<TestBench::WetterTyp> wetter;

    //SALATFELD
    sc_fifo_out<bool> salatfeld_blattlaeuse;
    sc_fifo_out<bool> salatfeld_schnecken;

    //KÜRBISFELD
    sc_fifo_out<bool> kuerbisfeld_blattlaeuse;

    //WASSERTANK
    sc_fifo_out<int> wassertank_auffuellen;

    // ------------------- In  Events -------------------

    //SALATFELD
    sc_fifo_in<bool> salatfeld_bewaessern;
    sc_fifo_in<bool> salatfeld_duengen;
    sc_fifo_in<bool> salatfeld_pflanzenschutz;

    //KÜRBISFELD
    sc_fifo_in<bool> kuerbisfeld_bewaessern;
    sc_fifo_in<bool> kuerbisfeld_duengen;
    sc_fifo_in<bool> kuerbisfeld_pflanzenschutz;

    //GEWÄCHSHAUS
    sc_fifo_in<bool> gewaechshaus_bewaessern;
    sc_fifo_in<bool> gewaechshaus_duengen;
    sc_in<int> gewaechshaus_heizung;
    sc_in<int> gewaechshaus_lampe;

    //WASSERTANK
    sc_in<bool> wassertank_warnlampe;
   
   
    SC_HAS_PROCESS(TestBench);
    TestBench(sc_module_name name, string test_case_name);

    typedef void (TestBench::*FnPtr)(void);
    FnPtr my_test_case;
    void thread_loop();
    void timeout();


private:
    void testBewaesserungOhneRegen();
    void testBewaesserung30Grad();
    void testBewaesserungMitPreRegen();
    void testBewaesserungMitMidRegen();
    void testBewaesserungMitDauerRegen();
    void testBlattlaeuseKuerbisfeld();
    void testBlattlaeuseSalatfeld();
    void testSchneckenSalatfeld();
    void testWarnlampeAusAn();
    void testWarnlampeRegenLoop();
    void testOverflowBauer();
    void testDuengungOhneRegen();
    void testDuengungMitRegen();
    void testDuengungMitDauerRegen();
    void testDuengungNachRegen();
    void testTemperaturUeber30GradSonnig();
    void testTemperaturUeber30GradRegen();
    void testTemperaturUeber30GradBewoelkt();
    void testTemperaturUeber30GradStarkBewoelkt();
    void testTemperaturUeber30GradDunkel();
    void testTemperaturUnter30GradSonnig();
    void testTemperaturUnter30GradRegen();
    void testTemperaturUnter30GradBewoelkt();
    void testTemperaturUnter30GradStarkBewoelkt();
    void testTemperaturUnter30GradDunkel();
    void testLampeOhneRegenSonnig();
    void testLampeOhneRegenBewoelkt();
    void testLampeOhneRegenStarkBewoelkt();
    void testLampeOhneRegenDunkel();
    void testLampeMitRegenSonnig();
    void testLampeMitRegenbewoelkt();
    void testLampeMitRegenStarkBewoelkt();
    void testLampeMitRegenDunkel();
    void testLampeMitUndOhneRegen();
};

#endif