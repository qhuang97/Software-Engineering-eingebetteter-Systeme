#include "systemc.h"
#include "TestBench.h"
#include <string>

#include "Bewaesserungssystem.h"
#include "Pflanzenschutzsystem.h"
#include "Duengungssystem.h"
#include "Klimasteuerung.h"
#include "Lampe.h"

bool stark_dun=0;


int sc_main(int argc, char *argv[])
{
    // ------------------- In  Events -------------------
    
    //WETTER
    sc_signal<bool> regen;
    sc_signal<int> temperatur;
    sc_signal<TestBench::WetterTyp> wetter;

    //SALATFELD
    sc_fifo<bool> salatfeld_blattlaeuse(1);
    sc_fifo<bool> salatfeld_schnecken(1);

    //KÜRBISFELD
    sc_fifo<bool> kuerbisfeld_blattlaeuse(1);

    //WASSERTANK
    sc_fifo<int> wassertank_auffuellen(1);

    // ------------------- Out Events -------------------

    //SALATFELD
    sc_fifo<bool> salatfeld_bewaessern(1);
    sc_fifo<bool> salatfeld_duengen(1);
    sc_fifo<bool> salatfeld_pflanzenschutz(1);

    //KÜRBISFELD
    sc_fifo<bool> kuerbisfeld_bewaessern(1);
    sc_fifo<bool> kuerbisfeld_duengen(1);
    sc_fifo<bool> kuerbisfeld_pflanzenschutz(1);

    //GEWÄCHSHAUS
    sc_fifo<bool> gewaechshaus_bewaessern(1);
    sc_fifo<bool> gewaechshaus_duengen(1);
    sc_signal<int> gewaechshaus_heizung;
    sc_signal<int> gewaechshaus_lampe;

    //WASSERTANK
    sc_signal<bool> wassertank_warnlampe;
	
	// Connect testbench ports to channels:
	TestBench tb("TestBench", argv[1]);
	
    tb.regen(regen);
    tb.temperatur(temperatur);
    tb.wetter(wetter);

    tb.salatfeld_bewaessern(salatfeld_bewaessern);
    tb.salatfeld_duengen(salatfeld_duengen);
    tb.salatfeld_pflanzenschutz(salatfeld_pflanzenschutz);
    tb.salatfeld_blattlaeuse(salatfeld_blattlaeuse);
    tb.salatfeld_schnecken(salatfeld_schnecken);

    tb.kuerbisfeld_bewaessern(kuerbisfeld_bewaessern);
    tb.kuerbisfeld_duengen(kuerbisfeld_duengen);
    tb.kuerbisfeld_pflanzenschutz(kuerbisfeld_pflanzenschutz);
    tb.kuerbisfeld_blattlaeuse(kuerbisfeld_blattlaeuse);

    tb.gewaechshaus_bewaessern(gewaechshaus_bewaessern);
    tb.gewaechshaus_duengen(gewaechshaus_duengen);
    tb.gewaechshaus_heizung(gewaechshaus_heizung);
    tb.gewaechshaus_lampe(gewaechshaus_lampe);
    tb.wassertank_auffuellen(wassertank_auffuellen);
    tb.wassertank_warnlampe(wassertank_warnlampe);

    // TODO connect your module(s) to channels

    Bewaesserungssystem bewaesserungssystem ("bewaesserungssystem");
    bewaesserungssystem.salatfeld_bewaessern(salatfeld_bewaessern);
    bewaesserungssystem.kuerbisfeld_bewaessern(kuerbisfeld_bewaessern);
    bewaesserungssystem.gewaechshaus_bewaessern(gewaechshaus_bewaessern);
    bewaesserungssystem.temperatur(temperatur);
    bewaesserungssystem.regen(regen);
    bewaesserungssystem.wassertank_auffuellen(wassertank_auffuellen);
    bewaesserungssystem.wassertank_warnlampe(wassertank_warnlampe);


    Pflanzenschutzsystem pflanzenschutzsystem ("pflanzenschutzsystem");
    pflanzenschutzsystem.salatfeld_blattlaeuse(salatfeld_blattlaeuse);
    pflanzenschutzsystem.salatfeld_schnecken(salatfeld_schnecken);
    pflanzenschutzsystem.salatfeld_pflanzenschutz(salatfeld_pflanzenschutz);
    pflanzenschutzsystem.kuerbisfeld_blattlaeuse(kuerbisfeld_blattlaeuse);
    pflanzenschutzsystem.kuerbisfeld_pflanzenschutz(kuerbisfeld_pflanzenschutz);

    Duengungssystem duengungssystem ("duengungssystem");
    duengungssystem.salatfeld_duengen(salatfeld_duengen);
    duengungssystem.kuerbisfeld_duengen(kuerbisfeld_duengen);
    duengungssystem.gewaechshaus_duengen(gewaechshaus_duengen);
    duengungssystem.regen(regen);

    Klimasteuerung klimasteuerung ("klimasteuerung");
    klimasteuerung.gewaechshaus_heizung(gewaechshaus_heizung);
    klimasteuerung.temperatur(temperatur);
    klimasteuerung.regen(regen);
    klimasteuerung.wetter(wetter);

    Lampe lampe ("lampe");
    lampe.gewaechshaus_lampe(gewaechshaus_lampe);
    lampe.temperatur(temperatur);
    lampe.regen(regen);
    lampe.wetter(wetter);

	// Run simulation:
    // NICHT ÄNDERN
	sc_start(3800, SC_SEC);
	return 0;
}
