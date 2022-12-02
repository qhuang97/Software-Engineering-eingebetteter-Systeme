cmake_minimum_required(VERSION 3.0.0)
project(ha2 VERSION 0.1.0)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(ha2 ${SOURCES})

set(SYSTEMC_PATH $ENV{SYSTEMC_HOME})
target_include_directories(ha2 PRIVATE ${SYSTEMC_PATH}/include)
find_library(SYSTEMC_LIBRARY NAMES systemc
    PATHS ${SYSTEMC_PATH}/lib-linux64 ${SYSTEMC_PATH}/lib-linux ${SYSTEMC_PATH}/lib
)
target_link_libraries(ha2 ${SYSTEMC_LIBRARY})
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)


enable_testing()
set(TEST_CASES 
    testBewaesserungOhneRegen;
    testBewaesserung30Grad;
    testBewaesserungMitPreRegen;
    testBewaesserungMitMidRegen;
    testBewaesserungMitDauerRegen;
    testBlattlaeuseKuerbisfeld;
    testBlattlaeuseSalatfeld;
    testSchneckenSalatfeld;
    testWarnlampeAusAn;
    testWarnlampeRegenLoop;
    testOverflowBauer;
    testDuengungOhneRegen;
    testDuengungMitRegen;
    testDuengungMitDauerRegen;
    testDuengungNachRegen;
    testTemperaturUeber30GradSonnig;
    testTemperaturUeber30GradRegen;
    testTemperaturUeber30GradBewoelkt;
    testTemperaturUeber30GradStarkBewoelkt;
    testTemperaturUeber30GradDunkel;
    testTemperaturUnter30GradSonnig;
    testTemperaturUnter30GradRegen;
    testTemperaturUnter30GradBewoelkt;
    testTemperaturUnter30GradStarkBewoelkt;
    testTemperaturUnter30GradDunkel;
    testLampeOhneRegenSonnig;
    testLampeOhneRegenBewoelkt;
    testLampeOhneRegenStarkBewoelkt;
    testLampeOhneRegenDunkel;
    testLampeMitRegenSonnig;
    testLampeMitRegenbewoelkt;
    testLampeMitRegenStarkBewoelkt;
    testLampeMitRegenDunkel;
    testLampeMitUndOhneRegen;
)

foreach(test_case ${TEST_CASES})
    add_test(NAME ${test_case} COMMAND ha2 ${test_case})
endforeach()
