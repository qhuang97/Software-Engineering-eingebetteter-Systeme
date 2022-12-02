#include "TestBench.h"
#include "TestHelper.h"
#include <map>
#include <string>

/*
 * Das sind die Testfälle für das Basisverhalten.
 * Sie sind den Basistests der ersten Hausaufgabe sehr ähnlich.
 * Wenn euer Modell nicht alle hier aufgeführten Testfälle besteht, bekommt ihr 0 Punkte.
 * 
 * Um die Tests auzuführen, müsst ihr folgende Befehle im Terminal eingeben:
 * 
 * cmake CMakeLists.txt     //erzeugt und konfiguriert Makefile
 * make                     //Führt Makefile aus und kompiliert euer Projekt
 * ctest                    //Führt euer Projekt mit den Tests aus
 * 
 * Wenn ihr nur einen einzigen Test ausführen wollt, kompiliert euer Projekt (also cmake, make ausführen) und gebt folgenden Befehl ein:
 * 
 * ./ha2 testName
 * 
 * Beispiel:
 * 
 * ./ha2 testBewaesserungOhneRegen         //Führt testBewaesserungOhneRegen aus
*/

bool finished = false;

TestBench::TestBench(sc_module_name name, string test_case_name) : sc_module(name)
{
    // Initialize signals:
    regen.initialize(false);
    temperatur.initialize(20);
    wetter.initialize(TestBench::sonnig);

    // Init test list:
    map<std::string, FnPtr> test_cases;
    
    test_cases["testBewaesserungOhneRegen"] = &TestBench::testBewaesserungOhneRegen;
    test_cases["testBewaesserung30Grad"] = &TestBench::testBewaesserung30Grad;
    test_cases["testBewaesserungMitPreRegen"] = &TestBench::testBewaesserungMitPreRegen;
    test_cases["testBewaesserungMitMidRegen"] = &TestBench::testBewaesserungMitMidRegen;
    test_cases["testBewaesserungMitDauerRegen"] = &TestBench::testBewaesserungMitDauerRegen;
    test_cases["testWarnlampeAusAn"] = &TestBench::testWarnlampeAusAn;
    test_cases["testWarnlampeRegenLoop"] = &TestBench::testWarnlampeRegenLoop;
    test_cases["testOverflowBauer"] = &TestBench::testOverflowBauer;
    test_cases["testBlattlaeuseKuerbisfeld"] = &TestBench::testBlattlaeuseKuerbisfeld;
    test_cases["testBlattlaeuseSalatfeld"] = &TestBench::testBlattlaeuseSalatfeld;
    test_cases["testSchneckenSalatfeld"] = &TestBench::testSchneckenSalatfeld;
    test_cases["testDuengungOhneRegen"] = &TestBench::testDuengungOhneRegen;
    test_cases["testDuengungMitRegen"] = &TestBench::testDuengungMitRegen;
    test_cases["testDuengungMitDauerRegen"] = &TestBench::testDuengungMitDauerRegen;
    test_cases["testDuengungNachRegen"] = &TestBench::testDuengungNachRegen;
    test_cases["testTemperaturUeber30GradSonnig"] = &TestBench::testTemperaturUeber30GradSonnig;
    test_cases["testTemperaturUeber30GradRegen"] = &TestBench::testTemperaturUeber30GradRegen;
    test_cases["testTemperaturUeber30GradBewoelkt"] = &TestBench::testTemperaturUeber30GradBewoelkt;
    test_cases["testTemperaturUeber30GradStarkBewoelkt"] = &TestBench::testTemperaturUeber30GradStarkBewoelkt;
    test_cases["testTemperaturUeber30GradDunkel"] = &TestBench::testTemperaturUeber30GradDunkel;
    test_cases["testTemperaturUnter30GradSonnig"] = &TestBench::testTemperaturUnter30GradSonnig;
    test_cases["testTemperaturUnter30GradRegen"] = &TestBench::testTemperaturUnter30GradRegen;
    test_cases["testTemperaturUnter30GradBewoelkt"] = &TestBench::testTemperaturUnter30GradBewoelkt;
    test_cases["testTemperaturUnter30GradStarkBewoelkt"] = &TestBench::testTemperaturUnter30GradStarkBewoelkt;
    test_cases["testTemperaturUnter30GradDunkel"] = &TestBench::testTemperaturUnter30GradDunkel;
    test_cases["testLampeOhneRegenSonnig"] = &TestBench::testLampeOhneRegenSonnig;
    test_cases["testLampeOhneRegenBewoelkt"] = &TestBench::testLampeOhneRegenBewoelkt;
    test_cases["testLampeOhneRegenStarkBewoelkt"] = &TestBench::testLampeOhneRegenStarkBewoelkt;
    test_cases["testLampeOhneRegenDunkel"] = &TestBench::testLampeOhneRegenDunkel;
    test_cases["testLampeMitRegenSonnig"] = &TestBench::testLampeMitRegenSonnig;
    test_cases["testLampeMitRegenbewoelkt"] = &TestBench::testLampeMitRegenbewoelkt;
    test_cases["testLampeMitRegenStarkBewoelkt"] = &TestBench::testLampeMitRegenStarkBewoelkt;
    test_cases["testLampeMitRegenDunkel"] = &TestBench::testLampeMitRegenDunkel;
    test_cases["testLampeMitUndOhneRegen"] = &TestBench::testLampeMitUndOhneRegen;
    
    // Select test function:
    if (test_cases.count(test_case_name) == 0)
    {
        cout << "Unknown test function '" << test_case_name << "'" << endl;
        exit(2);
    }
    my_test_case = test_cases[test_case_name];
    SC_THREAD(thread_loop);
    SC_THREAD(timeout);
}

void TestBench::thread_loop()
{
    finished = false;
    (this->*my_test_case)();
    finished = true;
    cout << "\n" <<  sc_time_stamp() << "\n\nTest PASSED\n" << endl;
}

void TestBench::timeout()
{        
    wait(3700, SC_SEC);
    if (not finished)
    {
        cout << "Test failed: Timeout" << endl;
        exit(1);
    }
}

// Test cases

//=========================================================================================================================
//Bewässerungssystem (Bewässerung)
//=========================================================================================================================

//test 1
    /* Wir testen, ob die Pflanzen ohne Regen korrekt bewässert werden.
     * 
     * -Kürbisse: Kürbisse haben große Blätter und verbrauchen insbesondere an warmen Tagen sehr viel Flüssigkeit. 
     * -Deswegen wird das Gießintervall abhängig von der Außentemperatur gesteuert:
     *  ∗ Beträgt die Außentemperatur zum Zeitpunkt der Bewässerung mehr als 30°, wird erneut nach 6 Minuten bewässert.
     *  ∗ Beträgt die Außentemperatur zum Zeitpunkt der Bewässerung 30°oder weniger, wird erneut nach 8 Minuten bewässert.
     * 
     * -Feldsalat: Feldsalat hat kleinere Blätter als der Kürbis. Er benötigt alle 11 Minuten Wasser, unabhängig von der Außentemperatur.
     * -Auberginen: Auberginen haben einen enormen Wasserbedarf und müssen alle 95 Sekunden bewässert werden.
     */
void TestBench::testBewaesserungOhneRegen(){
    int i = 0;                                                                                                      //Laufvariable für Schleife
    while (i < 3600){                                                                                               //3600 Iterationen um eine Stunde zu simulieren
        if (i > 0){                                                                                                 //nullten Zeitschritt überspringen
            if (i % 95 == 0){                                                                                       //alle 95 Sekunden sollen die Auberginen bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");            //Bewässerung der Auberginen prüfen
            }
            if (i % (8 * 60) == 0){                                                                                 //alle 8 Minuten sollen die Kürbisse bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");              //Bewässerung der Kürbisse prüfen
            }
            if (i % (11 * 60) == 0){                                                                                //alle 11 Minuten soll der Feldsalat bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &salatfeld_bewaessern, "salatfeld_bewaessern");                  //Bewässerung des Feldsalats prüfen
            }
        }
        i = i + 1;                                                                                                  //Laufvariable hochzählen
    }  
}

//test 2
    /* Wir testen, ob die Kürbisse ohne Regen bei 33°C korrekt bewässert werden.
     * 
     * -Kürbisse: Kürbisse haben große Blätter und verbrauchen insbesondere an warmen Tagen sehr viel Flüssigkeit. 
     * -Deswegen wird das Gießintervall abhängig von der Außentemperatur gesteuert:
     *  ∗ Beträgt die Außentemperatur zum Zeitpunkt der Bewässerung mehr als 30°, wird erneut nach 6 Minuten bewässert.
     *  ∗ Beträgt die Außentemperatur zum Zeitpunkt der Bewässerung 30°oder weniger, wird erneut nach 8 Minuten bewässert.
     */
void TestBench::testBewaesserung30Grad(){
    assert_at_time(sc_time(480, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");                        //Simulationszeit: 08:00.0 Kürbisse müssen bewässert werden
    assert_at_time(sc_time(960, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");                        //Simulationszeit: 16:00.0 Kürbisse müssen bewässert werden
    wait(61, SC_SEC);                                                                                               //Simulationszeit: 17:01.0
    send_signal(&temperatur, "temperatur", 33);                                                                     //es sind ab jetzt 33°C
    assert_at_time(sc_time(1440, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");                       //Simulationszeit: 24:00.0 Kürbisse müssen bewässert werden
    assert_at_time(sc_time(1800, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");                       //Simulationszeit: 30:00.0 Kürbisse müssen bewässert werden
}



//test 3
    /* Wir testen, ob die Pflanzen mit vorangegangem Regen korrekt bewässert werden.
     * -Das Bewässerungssystem (BWS) kann für Kürbisse und Feldsalat aktiv und inaktiv sein. 
     * -Wenn es regnet, ist das BWS auf den Feldern inaktiv. Regnet es nicht, ist es dort aktiv. 
     * -Im Gewächshaus ist das BWS immer aktiv.
     * -Die Gießintervalle beginnen immer mit Aktivierung des BWS für das jeweilige Gemüse. 
     * -Beispiel: es hört soeben auf zu regnen. Der Regen beginnt danach nicht erneut. 
     * -Das BWS für den Feldsalat wird mit Ende des Regens aktiv und bewässert den Salat 11, 22, 33, usw. Minuten nach Ende des Regens.
     */
void TestBench::testBewaesserungMitPreRegen(){
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(3, SC_SEC);                                                                                                //3 Sekunden warten
    send_signal(&regen, "regen", false);                                                                            //es hört auf zu regnen

    int i = 3;                                                                                                      //Laufvariable für Schleife
    while (i < 3003){                                                                                               //3000 Iterationen

        if ((i - 3) > 0){                                                                                           //nullte Iteration überspringen

            if (i % 95 == 0){                                                                                       //alle 95 Sekunden sollen die Auberginen bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");            //Bewässerung der Auberginen prüfen
            }
            if ((i - 3) % (8 * 60) == 0){                                                                           //alle 8 Minuten sollen die Kürbisse bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");              //Bewässerung der Kürbisse prüfen
            }
            if ((i - 3) % (11 * 60) == 0){                                                                          //alle 11 Minuten soll der Feldsalat bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &salatfeld_bewaessern, "salatfeld_bewaessern");                  //Bewässerung des Feldsalats prüfen
            }
        }
        i = i + 1;                                                                                                  //Laufvariable hochzählen
    }
}

//test 4
    /* Wir testen, ob die Pflanzen mit Regen zwischendurch korrekt bewässert werden.
     * -Das Bewässerungssystem (BWS) kann für Kürbisse und Feldsalat aktiv und inaktiv sein. 
     * -Wenn es regnet, ist das BWS auf den Feldern inaktiv. Regnet es nicht, ist es dort aktiv. 
     * -Im Gewächshaus ist das BWS immer aktiv.
     * -Die Gießintervalle beginnen immer mit Aktivierung des BWS für das jeweilige Gemüse. 
     * -Beispiel: es hört soeben auf zu regnen. Der Regen beginnt danach nicht erneut. 
     * -Das BWS für den Feldsalat wird mit Ende des Regens aktiv und bewässert den Salat 11, 22, 33, usw. Minuten nach Ende des Regens.
     */
void TestBench::testBewaesserungMitMidRegen(){
    
    int i = 0;                                                                                                      //Laufvariable für Schleife
    while (i < 665){                                                                                                //665 Iterationen
        if (i > 0){                                                                                                 //nullten Zeitschritt überspringen
            if (i % 95 == 0){                                                                                       //alle 95 Sekunden sollen die Auberginen bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");            //Bewässerung der Auberginen prüfen
            }
            if (i % (8 * 60) == 0){                                                                                 //alle 8 Minuten sollen die Kürbisse bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");              //Bewässerung der Kürbisse prüfen
            }
            if (i % (11 * 60) == 0){                                                                                //alle 11 Minuten soll der Feldsalat bewässert werden
                assert_at_time(sc_time(i, SC_SEC), &salatfeld_bewaessern, "salatfeld_bewaessern");                  //Bewässerung des Feldsalats prüfen
            }
        }
        i = i + 1;                                                                                                  //Laufvariable hochzählen
    }    
    assert_at_time(sc_time(665, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");                      //Auberginen sollten nach 665s bewässert werden
    wait(100, SC_MS);                                                                                               //100ms warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    assert_at_time(sc_time(760, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");                      //Auberginen sollten nach 760s bewässert werden
    send_signal(&regen, "regen", false);                                                                            //es hört auf zu regnen
    
    i = 0;                                                                                                          //Laufvariable für Schleife
    while (i < 665){                                                                                                //665 Iterationen
        if (i > 0){                                                                                                 //nullten Zeitschritt überspringen
            if (i % 95 == 0){                                                                                       //alle 95 Sekunden sollen die Auberginen bewässert werden
                assert_at_time(sc_time(i + 760, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");      //Bewässerung der Auberginen prüfen
            }
            if (i % (8 * 60) == 0){                                                                                 //alle 8 Minuten sollen die Kürbisse bewässert werden
                assert_at_time(sc_time(i + 760, SC_SEC), &kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern");        //Bewässerung der Kürbisse prüfen
            }
            if (i % (11 * 60) == 0){                                                                                //alle 11 Minuten soll der Feldsalat bewässert werden
                assert_at_time(sc_time(i + 760, SC_SEC), &salatfeld_bewaessern, "salatfeld_bewaessern");            //Bewässerung des Feldsalats prüfen
            }
        }
        i = i + 1;                                                                                                  //Laufvariable hochzählen
    }   
}

//test 5
    /* Wir testen, ob die Pflanzen mit dauerhaftem Regen korrekt bewässert werden.
     * -Das Bewässerungssystem (BWS) kann für Kürbisse und Feldsalat aktiv und inaktiv sein. 
     * -Wenn es regnet, ist das BWS auf den Feldern inaktiv. Regnet es nicht, ist es dort aktiv. 
     * -Im Gewächshaus ist das BWS immer aktiv.
     */
void TestBench::testBewaesserungMitDauerRegen(){
    
    wait(100, SC_MS);                                                                                               //Simulationszeit: 00:00.1
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(900, SC_MS);                                                                                               //Simulationszeit: 00:01.0

    int i = 0;                                                                                                      //Laufvariable für Schleife
    while (i < 3600){                                                                                               //3600 Iterationen um eine Stunde zu simulieren
        if (i > 1){                                                                                                 //nullte Iteration überspringen
            if ((i + 1) % 95 == 0){                                                                                 //alle 95s sollten die Auberginen bewässert werden
                assert_at_time(sc_time(i + 1, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");        //Bewässerung der Auberginen prüfen
            }   
            if ((i + 1) % (8 * 60) == 0){                                                                           //alle 8 Minuten sollten die Kürbisse eigentlich bewässert werden 
                assert_now(&kuerbisfeld_bewaessern, "kuerbisfeld_bewaessern", false);                               //bei Regen sollen die Kürbisse nicht bewässert werden
            }
            if ((i + 1) % (11 * 60) == 0){                                                                          //alle 11 Minuten sollte eigentlich der Feldsalat bewässert werden
                assert_now(&salatfeld_bewaessern, "salatfeld_bewaessern", false);                                   //bei Regen soll der Feldsalat nicht bewässert werden
            }
        }
        wait(1, SC_SEC);                                                                                            //eine Sekunde vergeht
        i = i + 1;                                                                                                  //Laufvariable hochzählen
    }  

}

//=========================================================================================================================
//Bewässerungssystem (Wassertank)
//=========================================================================================================================

//test 6
	/* Wir testen, ob die Warnlampe bei 500ml an ist und bei 1000ml aus ist.
	 * -Wenn der Füllstand des Tanks unter 1000 ml liegt, soll eine Warnlampe leuchten (Wassertank.warnlampe = true). 
	 * -Beträgt der Füllstand mindestens 1 L, soll die Warnlampe nicht leuchten (Wassertank.warnlampe = false)
	 */
void TestBench::testWarnlampeAusAn(){ 
    wassertank_auffuellen.write(500);					                                                            //500ml auffüllen, Tankinhalt ist 500ml
    wait(100, SC_MS);										                                                        //kurz warten
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);					                            //Tankinhalt ist 500ml => die Warnlampe sollte an sein
	wait(100, SC_MS);										                                                        //kurz warten
	wassertank_auffuellen.write(500);					                                                            //500ml auffüllen, Tankinhalt ist 1000ml
    wait(100, SC_MS);										                                                        //kurz warten
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);				                                //Tankinhalt ist 1000ml => die Warnlampe sollte aus sein
}

//test 7
	/* Wir testen anhand der Warnlampe, ob sich der Tank korrekt mit Wasser füllt.
	 * -Jede Bewässerung verbraucht 1000 ml.
	 * -Beginnt es zu regnen, füllt sich der Wassertank alle 95 Sekunden um 100 ml, bis der Regen endet.
	 * -Auberginen haben einen enormen Wasserbedarf und müssen alle 95 Sekunden bewässert werden.
	 */
void TestBench::testWarnlampeRegenLoop(){ 
    assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);					                            //Tankinhalt ist 0ml => die Warnlampe sollte an sein
	wait(100, SC_MS);										                                                        //kurz warten
	send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(1, SC_MS);										                                                            //kurz warten
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);					                            //Tankinhalt ist 0ml => die Warnlampe sollte an sein
		
	int i = 0;									                                                                    //Laufvariable für Schleife
	while (i < 9) {										                                                            //9 Iterationen
		wassertank_auffuellen.write(1000);				                                                            //1000ml für Auberginenbewässerung
        wait(1, SC_MS);
		assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);			                                //Tankinhalt ist mind. 1000ml => die Warnlampe sollte aus sein
		wait(95, SC_SEC);										                                                    //95s warten, -1000ml (Auberginen werden bewässert), +100ml (Regen)
		assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);				                            //Tankinhalt is (i+1) * 100ml <= 900ml => die Warnlampe sollte immer aus sein
		i = i + 1;										                                                            //Zählervariable erhöhen
	}
		
	wassertank_auffuellen.write(1000);					                                                            //1000ml für Auberginenbewässerung
    wait(1, SC_MS);                                                                                                 //1ms warten
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);				                                //Tankinhalt ist 1900ml => die Warnlampe sollte aus sein
	wait(95, SC_SEC);												                                                //95s warten, -1000ml (Auberginen werden bewässert), +100ml (Regen)
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);				                                //Tankinhalt ist 1000ml => die Warnlampe sollte aus sein
}

//test 8
	/* Wir testen, anhand der Warnlampe, ob der Tank wirklich nur bis 5000ml gefüllt werden kann.
	 * -Der Wassertank hat eine Kapazität von 5000 ml. Initial ist der Tank leer.
	 */
void TestBench::testOverflowBauer(){ 
    wassertank_auffuellen.write(500);					                                                            //500ml auffüllen, Tankinhalt ist 500ml
	wait(100, SC_MS);										                                                        //kurz warten
	wassertank_auffuellen.write(1000);					                                                            //1000ml auffüllen, Tankinhalt ist 1500ml
	wait(100, SC_MS);										                                                        //kurz warten
	wassertank_auffuellen.write(1000);					                                                            //1000ml auffüllen, Tankinhalt ist 2500ml
	wait(100, SC_MS);										                                                        //kurz warten
	wassertank_auffuellen.write(1000);					                                                            //1000ml auffüllen, Tankinhalt ist 3500ml
	wait(100, SC_MS);										                                                        //kurz warten
	wassertank_auffuellen.write(1000);					                                                            //1000ml auffüllen, Tankinhalt ist 4500ml
	wait(100, SC_MS);										                                                        //kurz warten
	wassertank_auffuellen.write(1000);					                                                            //1000ml auffüllen, Tankinhalt ist 5000ml
	wait(500, SC_MS);										                                                        //kurz warten
	wait(94, SC_SEC);											                                                    //nach 95s sollten die Auberginen bewässert werden, Tankinhalt 4000ml
	assert_at_time(sc_time(95, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");					    //Auberginen sollten jetzt bewässert werden								                                                //nach 2 * 95s = 190s sollten die Auberginen bewässert werden, Tankinhalt 3000ml
	assert_at_time(sc_time(190, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");					    //Auberginen sollten jetzt bewässert werden										                                                //nach 3 * 95s = 285s sollten die Auberginen bewässert werden, Tankinhalt 2000ml
	assert_at_time(sc_time(285, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");					    //Auberginen sollten jetzt bewässert werden

    assert_at_time(sc_time(380, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");					    //Auberginen sollten jetzt bewässert werden

    wait(1, SC_MS);
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);				                                //Tankinhalt = 1000ml => die Warnlampe sollte aus sein
	assert_at_time(sc_time(475, SC_SEC), &gewaechshaus_bewaessern, "gewaechshaus_bewaessern");				        //Auberginen sollten jetzt bewässert werden
		
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);					                            //Tankinhalt ist 0ml => die Warnlampe sollte an sein
	wait(99, SC_MS);										                                                        //100ms warten um Signalkollisionen zu vermeiden
	send_signal(&regen, "regen", true);							                                                    //es beginnt zu regnen
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);				                                //Tankinhalt ist 0ml => die Warnlampe sollte an sein

	int i = 0;									                                                                    //Laufvariable für Schleife
	while (i < 9) {										                                                            //9 Iterationen
		wassertank_auffuellen.write(1000);				                                                            //1000ml für Auberginenbewässerung
        wait(1, SC_MS);                                                                                             //1ms warten
		assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);			                                //Tankinhalt ist mind. 1000ml => die Warnlampe sollte aus sein
		wait(95, SC_SEC);										                                                    //95s warten, -1000ml (Auberginen werden bewässert), +100ml (Regen)
		assert_now(&wassertank_warnlampe, "wassertank_warnlampe", true);				                            //Tankinhalt is (i+1) * 100ml <= 900ml => die Warnlampe sollte immer aus sein
		i = i + 1;										                                                            //Zählervariable erhöhen
	}
		
	wassertank_auffuellen.write(1000);					                                                            //1000ml für Auberginenbewässerung
    wait(1, SC_MS);                                                                                                 //eine Sekunde warten
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);				                                //Tankinhalt ist 1900ml => die Warnlampe sollte aus sein
	wait(95, SC_SEC);												                                                //95s warten, -1000ml (Auberginen werden bewässert), +100ml (Regen)
	assert_now(&wassertank_warnlampe, "wassertank_warnlampe", false);				                                //Tankinhalt ist 1000ml => die Warnlampe sollte aus sein
}

//=========================================================================================================================
//Pflanzenschutzsystem
//=========================================================================================================================

//test 9
	/* Wir testen, ob auf dem Kürbisfeld bei Blattlausbefall der Pflanzenschutz korrekt versprüht wird.
	 * -hin und wieder werden Kürbisse von Blattläusen befallen. Wird ein Befall gemeldet, wird sofort Pflanzenschutzmittel eingesetzt.
	 */
void TestBench::testBlattlaeuseKuerbisfeld(){
    wait(10, SC_SEC);                                                                                               //10 Sekunden warten
    send_signal_fifo(&kuerbisfeld_blattlaeuse, "kuerbisfeld_blattlaeuse", true);                                    //Blattlausbefall auf Kürbisfeld wird gemeldet!
    assert_at_time(sc_time(10, SC_SEC), &kuerbisfeld_pflanzenschutz, "kuerbisfeld_pflanzenschutz");                 //Pflanzenschutz auf Kürbisfeld wird erwartet
}

//test 10
	/* Wir testen, ob auf dem Salatfeld bei Blattlausbefall der Pflanzenschutz korrekt versprüht wird.
	 * -Wird ein Befall mit Blattläusen gemeldet, wird sofort Pflanzenschutzmittel eingesetzt.
	 */
void TestBench::testBlattlaeuseSalatfeld(){
    wait(11, SC_SEC);                                                                                               //11 Sekunden warten
    send_signal_fifo(&salatfeld_blattlaeuse, "salatfeld_blattlaeuse", true);                                        //Blattlausbefall auf Salatfeld wird gemeldet!
    assert_at_time(sc_time(11, SC_SEC), &salatfeld_pflanzenschutz, "salatfeld_pflanzenschutz");                     //Pflanzenschutz auf Salatfeld wird erwartet
}   

//test 11
	/* Wir testen, ob auf dem Salatfeld bei Schneckenbefall der Pflanzenschutz korrekt versprüht wird.
	 * -Wird ein Befall mit Schnecken gemeldet, wird sofort und 20 Sekunden danach Pflanzenschutzmittel eingesetzt.
	 */
void TestBench::testSchneckenSalatfeld(){
    wait(12, SC_SEC);                                                                                               //12 Sekunden warten
    send_signal_fifo(&salatfeld_schnecken, "salatfeld_schnecken", true);                                            //Schneckenbefall wird gemeldet!
    assert_at_time(sc_time(12, SC_SEC), &salatfeld_pflanzenschutz, "salatfeld_pflanzenschutz");                     //direkter Pflanzenschutz auf Salatfeld wird erwartet
    assert_at_time(sc_time(32, SC_SEC), &salatfeld_pflanzenschutz, "salatfeld_pflanzenschutz");                     //20s später nochmal
}


//=========================================================================================================================
//Düngungssystem
//=========================================================================================================================	

//test 12
	/* Wir testen, ob die Pflanzen ohne Regen korrekt gedünkt werden.
	 * -Kürbisse: Kürbisse müssen alle 13 Minuten gedüngt werden.
	 * -Feldsalat: Feldsalat muss nur alle 25 Minuten gedüngt werden.
	 * -Auberginen: Auberginen müssen alle 17 Minuten gedüngt werden.
	 */
void TestBench::testDuengungOhneRegen(){
    int i = 0;                                                                                                      //Laufvariable für Schleife
    while (i < 3600){                                                                                               //3600 Iterationen um eine Stunde zu simulieren
        if (i > 0){                                                                                                 //nullten Zeitschritt überspringen
            if (i % (17 * 60) == 0){                                                                                //alle 17 Minuten sollen die Auberginen gedüngt werden
                assert_at_time(sc_time(i, SC_SEC), &gewaechshaus_duengen, "gewaechshaus_duengen");                  //Düngung der Auberginen prüfen
            }
            if (i % (13 * 60) == 0){                                                                                //alle 13 Minuten sollen die Kürbisse gedüngt werden
                assert_at_time(sc_time(i, SC_SEC), &kuerbisfeld_duengen, "kuerbisfeld_duengen");                    //Düngung der Kürbisse prüfen
            }
            if (i % (25 * 60) == 0){                                                                                //alle 25 Minuten soll der Feldsalat gedüngt werden
                assert_at_time(sc_time(i, SC_SEC), &salatfeld_duengen, "salatfeld_duengen");                        //Düngung des Feldsalats prüfen
            }
        }
        wait(1, SC_SEC);                                                                                            //eine Sekunde vergeht
        i = i + 1;                                                                                                  //Laufvariable erhöhen
    }  
}

//test 13
	/* Wir testen, ob die Pflanzen mit Regen korrekt bewässert werden.
	 * -Kürbisse sind Starkzehrer und müssen kontinuierlich mit Nährstoffen versorgt werden. 
	 * -Regnet es 10 Minuten ohne Unterbrechung, beginnt deswegen ein besonderer Starkdüngemodus. 
	 * -Dieser verhindert, dass die Nährstoffe aus dem Boden gewaschen werden:
	 * -Der Starkdüngemodus überschreibt das normale Düngeintervall. 
	 * -Er löst bei Aktivierung sofort eine Düngung aus und dann alle 5 Minuten.
	 * -Der Starkdüngemodus endet, sobald der Regen endet. 
	 * -Anders als das Gießintervall beim BWS wird das Düngungsintervall bei Ende des Regens nicht zurückgesetzt. 
	 * -Die nächste Düngung würde also 13 Minuten nach der letzten Düngung erfolgen, sofern es in dieser Zeit nicht mindestens 10 Minuten regnet.
	 */
void TestBench::testDuengungMitRegen(){
    assert_at_time(sc_time(780, SC_SEC), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							    //Kürbisse sollten nach 13 Minuten gedüngt werden
	assert_at_time(sc_time(1020, SC_SEC), &gewaechshaus_duengen, "gewaechshaus_duengen");							//Auberginen sollten nach 17 Minuten gedüngt werden
	assert_at_time(sc_time(1500, SC_SEC), &salatfeld_duengen, "salatfeld_duengen");							        //Feldsalat sollte nach 25 Minuten gedüngt werden
	wait(100, SC_MS);										                                                        //Simulationszeit: 25:00.1
	send_signal(&regen, "regen", true); 							                                                //es beginnt zu regnen
	assert_at_time(sc_time(1560, SC_SEC), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							    //Kürbisse sollten nach 26 Minuten gedüngt werden
	assert_at_time(sc_time(2040, SC_SEC), &gewaechshaus_duengen, "gewaechshaus_duengen");							//Auberginen sollten nach 34 Minuten gedüngt werden
	assert_at_time(sc_time(2100100, SC_MS), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							//Kürbisse sollte nach 10 Minuten regen gedüngt werden
	assert_at_time(sc_time(2400100, SC_MS), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							//Kürbisse sollten im Düngemodus nach 5 Minuten Regen gedüngt werden
}

//test 14
	/* Wir testen, ob die Pflanzen mit andauerndem Regen korrekt bewässert werden.
	 * -Kürbisse sind Starkzehrer und müssen kontinuierlich mit Nährstoffen versorgt werden. 
	 * -Regnet es 10 Minuten ohne Unterbrechung, beginnt deswegen ein besonderer Starkdüngemodus. 
	 * -Dieser verhindert, dass die Nährstoffe aus dem Boden gewaschen werden:
	 * -Der Starkdüngemodus überschreibt das normale Düngeintervall. 
	 * -Er löst bei Aktivierung sofort eine Düngung aus und dann alle 5 Minuten.
	 * -Der Starkdüngemodus endet, sobald der Regen endet. 
	 * -Anders als das Gießintervall beim BWS wird das Düngungsintervall bei Ende des Regens nicht zurückgesetzt. 
	 * -Die nächste Düngung würde also 13 Minuten nach der letzten Düngung erfolgen, sofern es in dieser Zeit nicht mindestens 10 Minuten regnet.
	 */
void TestBench::testDuengungMitDauerRegen(){

    wait(100, SC_MS);										                                                        //Simulationszeit: 00:00.1
	send_signal(&regen, "regen", true);							                                                    //es beginnt zu regnen
	assert_at_time(sc_time(600100, SC_MS), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							//Kürbisse sollte nach 10 Minuten regen gedüngt werden
	assert_at_time(sc_time(900100, SC_MS), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							//Kürbisse sollten im Düngemodus nach 5 Minuten Regen gedüngt werden
	assert_at_time(sc_time(1200100, SC_MS), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							//Kürbisse sollten im Düngemodus nach 5 Minuten Regen gedüngt werden
	assert_at_time(sc_time(1500, SC_SEC), &salatfeld_duengen, "salatfeld_duengen");							        //Feldsalatn sollte nach 25 Minuten gedüngt werden
	assert_at_time(sc_time(1500100, SC_MS), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							//Kürbisse sollten im Düngemodus nach 5 Minuten Regen gedüngt werden
}

//test 15
	/* Wir testen, ob die Pflanzen mit vorangegangem Regen korrekt bewässert werden.
	 * -Kürbisse sind Starkzehrer und müssen kontinuierlich mit Nährstoffen versorgt werden. 
	 * -Regnet es 10 Minuten ohne Unterbrechung, beginnt deswegen ein besonderer Starkdüngemodus. 
	 * -Dieser verhindert, dass die Nährstoffe aus dem Boden gewaschen werden:
	 * -Der Starkdüngemodus überschreibt das normale Düngeintervall. 
	 * -Er löst bei Aktivierung sofort eine Düngung aus und dann alle 5 Minuten.
	 * -Der Starkdüngemodus endet, sobald der Regen endet. 
	 * -Anders als das Gießintervall beim BWS wird das Düngungsintervall bei Ende des Regens nicht zurückgesetzt. 
	 * -Die nächste Düngung würde also 13 Minuten nach der letzten Düngung erfolgen, sofern es in dieser Zeit nicht mindestens 10 Minuten regnet.
	 */
void TestBench::testDuengungNachRegen(){
    send_signal(&regen, "regen", true);							                                                    //es beginnt zu regnen
	assert_at_time(sc_time(600, SC_SEC), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							    //Kürbisse sollte nach 10 Minuten regen gedüngt werden
	assert_at_time(sc_time(900, SC_SEC), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							    //Kürbisse sollten im Düngemodus nach 5 Minuten Regen gedüngt werden
	
    wait(1, SC_SEC);                                                                                                //Simulationszeit: 15:01.0
	send_signal(&regen, "regen", false);                                                                            //es hört auf zu regnen
	assert_at_time(sc_time(1680, SC_SEC), &kuerbisfeld_duengen, "kuerbisfeld_duengen");							    //13 Minuten nach ihrer letzten Düngung sollten die Kürbisse wieder gedüngt werden
}

//=========================================================================================================================
//Klimasteuerung (Heizung)
//=========================================================================================================================	

//test 16
    /* Wir testen, ob die Heizung bei einer Außentemperatur von über 30°C und bei sonnigem Wetter korrekt funktioniert.
	 * -Außentemperatur > 30 °C:
	 * 	-Wenn es sonnig oder bewölkt ist, soll nicht geheizt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 1 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 2 gewählt werden.
	 */
void TestBench::testTemperaturUeber30GradSonnig(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&temperatur, "temperatur", 31);                                                                     //Temperatur ist jetzt 31°C
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::sonnig);                                                   //das Wetter ist jetzt sonnig
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 0);                                                   //Heizung sollte aus sein
}

//test 17
    /* Wir testen, ob die Heizung bei einer Außentemperatur von über 30°C und bei Regen korrekt funktioniert.
	 * -Außentemperatur > 30 °C:
	 * 	-Wenn es sonnig oder bewölkt ist, soll nicht geheizt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 1 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 2 gewählt werden.
	 */
void TestBench::testTemperaturUeber30GradRegen(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&temperatur, "temperatur", 31);                                                                     //Temperatur ist jetzt 31°C
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 1);                                                   //Heizung sollte auf Stufe 1 sein
}

//test 18
    /* Wir testen, ob die Heizung bei einer Außentemperatur von über 30°C und bei bewölktem Wetter korrekt funktioniert.
	 * -Außentemperatur > 30 °C:
	 * 	-Wenn es sonnig oder bewölkt ist, soll nicht geheizt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 1 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 2 gewählt werden.
	 */
void TestBench::testTemperaturUeber30GradBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&temperatur, "temperatur", 31);                                                                     //Temperatur ist jetzt 31°C
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::bewoelkt);                                                 //das Wetter ist jetzt bewoelkt
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 0);                                                   //Heizung sollte aus sein
}

//test 19
    /* Wir testen, ob die Heizung bei einer Außentemperatur von über 30°C und bei stark bewölktem Wetter korrekt funktioniert.
	 * -Außentemperatur > 30 °C:
	 * 	-Wenn es sonnig oder bewölkt ist, soll nicht geheizt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 1 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 2 gewählt werden.
	 */
void TestBench::testTemperaturUeber30GradStarkBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&temperatur, "temperatur", 31);                                                                     //Temperatur ist jetzt 31°C
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::stark_bewoelkt);                                          //das Wetter ist jetzt stark bewoelkt
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 1);                                                   //Heizung sollte auf Stufe 1 sein
}

//test 20
    /* Wir testen, ob die Heizung bei einer Außentemperatur von über 30°C und bei Dunkelheit korrekt funktioniert.
	 * -Außentemperatur > 30 °C:
	 * 	-Wenn es sonnig oder bewölkt ist, soll nicht geheizt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 1 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 2 gewählt werden.
	 */
void TestBench::testTemperaturUeber30GradDunkel(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&temperatur, "temperatur", 31);                                                                     //Temperatur ist jetzt 31°C
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::dunkel);                                                   //das Wetter ist jetzt dunkel
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 2);                                                   //Heizung sollte auf Stufe 2 sein
}

//test 21
    /* Wir testen, ob die Heizung bei einer Außentemperatur von 20°C und bei sonnigem Wetter korrekt funktioniert.
	 * -Außentemperatur <= 30 °C:
	 * 	-Wenn es sonnig ist, soll Stufe 1 gewählt werden.
     *  -Wenn es bewölkt ist, soll Stufe 2 gewählt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 3 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 4 gewählt werden.
	 */
void TestBench::testTemperaturUnter30GradSonnig(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::sonnig);                                                   //das Wetter ist jetzt sonnig
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 1);                                                   //Heizung sollte auf Stufe 1 sein
}

//test 22
    /* Wir testen, ob die Heizung bei einer Außentemperatur von 20°C und bei Regen korrekt funktioniert.
	 * -Außentemperatur <= 30 °C:
	 * 	-Wenn es sonnig ist, soll Stufe 1 gewählt werden.
     *  -Wenn es bewölkt ist, soll Stufe 2 gewählt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 3 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 4 gewählt werden.
	 */
void TestBench::testTemperaturUnter30GradRegen(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 3);                                                   //Heizung sollte auf Stufe 3 sein
}

//test 23
    /* Wir testen, ob die Heizung bei einer Außentemperatur von 20°C und bei bewölktem Wetter korrekt funktioniert.
	 * -Außentemperatur <= 30 °C:
	 * 	-Wenn es sonnig ist, soll Stufe 1 gewählt werden.
     *  -Wenn es bewölkt ist, soll Stufe 2 gewählt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 3 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 4 gewählt werden.
	 */
void TestBench::testTemperaturUnter30GradBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::bewoelkt);                                                 //das Wetter ist jetzt bewoelkt
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 2);                                                   //Heizung sollte auf Stufe 2 sein
}

//test 24
    /* Wir testen, ob die Heizung bei einer Außentemperatur von 20°C und bei stark bewölktem Wetter korrekt funktioniert.
	 * -Außentemperatur <= 30 °C:
	 * 	-Wenn es sonnig ist, soll Stufe 1 gewählt werden.
     *  -Wenn es bewölkt ist, soll Stufe 2 gewählt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 3 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 4 gewählt werden.
	 */
void TestBench::testTemperaturUnter30GradStarkBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::stark_bewoelkt);                                          //das Wetter ist jetzt stark bewoelkt
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 3);                                                   //Heizung sollte auf Stufe 3 sein
}

//test 25
    /* Wir testen, ob die Heizung bei einer Außentemperatur von 20°C und bei dunklem Wetter korrekt funktioniert.
	 * -Außentemperatur <= 30 °C:
	 * 	-Wenn es sonnig ist, soll Stufe 1 gewählt werden.
     *  -Wenn es bewölkt ist, soll Stufe 2 gewählt werden.
	 ∗ 	-Wenn es regnet oder stark bewölkt ist, soll Stufe 3 gewählt werden.
	 ∗ 	-Wenn es dunkel ist, soll Stufe 4 gewählt werden.
	 */
void TestBench::testTemperaturUnter30GradDunkel(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::dunkel);                                                   //das Wetter ist jetzt dunkel
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_heizung, "gewaechshaus_heizung", 4);                                                   //Heizung sollte auf Stufe 4 sein
}

//=========================================================================================================================
//Klimasteuerung (Lampe)
//=========================================================================================================================	

//test 26
    /* Wir testen, ob die Lampe ohne Regen bei sonnigem Wetter korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeOhneRegenSonnig(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::sonnig);                                                   //das Wetter ist jetzt sonnig
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 0);                                                       //Lampe sollte auf Stufe 0 sein
}

//test 27
    /* Wir testen, ob die Lampe ohne Regen bei bewölktem Wetter korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeOhneRegenBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::bewoelkt);                                                 //das Wetter ist jetzt bewölkt
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 0);                                                       //Lampe sollte auf Stufe 0 sein
}

//test 28
    /* Wir testen, ob die Lampe ohne Regen bei stark bewölktem Wetter korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeOhneRegenStarkBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::stark_bewoelkt);                                           //das Wetter ist jetzt stark bewölkt
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 2);                                                       //Lampe sollte auf Stufe 2 sein
}

//test 29
    /* Wir testen, ob die Lampe ohne Regen bei Dunkelheit korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeOhneRegenDunkel(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::dunkel);                                                   //das Wetter ist jetzt dunkel
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 3);                                                       //Lampe sollte auf Stufe 3 sein
}

//test 30
    /* Wir testen, ob die Lampe bei sonnigem Wetter mit Regen korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeMitRegenSonnig(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::sonnig);                                                   //das Wetter ist jetzt sonnig
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 1);                                                       //Lampe sollte auf Stufe 1 sein
}

//test 31
    /* Wir testen, ob die Lampe bei bewölktem Wetter mit Regen korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeMitRegenbewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::bewoelkt);                                                 //das Wetter ist jetzt bewoelkt
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 1);                                                       //Lampe sollte auf Stufe 1 sein
}

//test 32
    /* Wir testen, ob die Lampe bei stark bewölktem Wetter mit Regen korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeMitRegenStarkBewoelkt(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::stark_bewoelkt);                                           //das Wetter ist jetzt stark bewoelkt
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 3);                                                       //Lampe sollte auf Stufe 3 sein
}

//test 33
    /* Wir testen, ob die Lampe bei Dunkelheit mit Regen korrekt eingestellt ist.
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0).
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2.
	 * -Wenn es regnet und es vorher stark bewölkt oder dunkel war, beträgt das Lichtlevel während des Regens 0.
	 * -Das tatsächliche Lichtlevel im Gewächshaus ergibt sich aus der Summe der Helligkeitsstufe der Lampe und des Lichtlevels im Gewächshaus. 
	 * -Das KS sorgt dafür, dass es zu jedem Zeitpunkt mindestens 3 beträgt. 
	 * -Um Energie zu sparen, sollte die Helligkeit der Lampe außerdem so niedrig wie möglich sein.
	 */
void TestBench::testLampeMitRegenDunkel(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::dunkel);                                                   //das Wetter ist jetzt dunkel
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 3);                                                       //Lampe sollte auf Stufe 3 sein
}

//test 34
    /* Wir testen, ob die Lampe auch nach ende des Regens noch korrekt eingestellt ist
	 * -Das aktuelle Wetter kann jederzeit durch Regen unterbrochen werden. Hört der Regen auf, tritt das vorherige Wetter wieder ein
	 * -sonnig (Lichtlevel 4), bewölkt (Lichtlevel 3), stark bewölkt (Lichtlevel 1) oder dunkel (Lichtlevel 0)
	 * -Wenn es regnet und es vorher sonnig oder bewölkt war, beträgt das Lichtlevel während des Regens 2..
	 */
void TestBench::testLampeMitUndOhneRegen(){
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&wetter, "wetter", TestBench::WetterTyp::sonnig);                                                   //das Wetter ist jetzt sonnig
    wait(100, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", true);                                                                             //es beginnt zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 1);                                                       //Lampe sollte auf Stufe 1 sein
    wait(600, SC_MS);                                                                                               //kurz warten
    send_signal(&regen, "regen", false);                                                                            //es hört auf zu regnen
    wait(100, SC_MS);                                                                                               //kurz warten
    assert_now(&gewaechshaus_lampe, "gewaechshaus_lampe", 0);                                                       //Lampe sollte auf Stufe 0 sein
}