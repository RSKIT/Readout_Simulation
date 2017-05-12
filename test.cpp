#include <iostream>
#include <string>

#include "evaluation.cpp"

int test()
{
    std::cout << "test" << std::endl;
    Evaluation asd = Evaluation();
    std::string event_gen = "event_gen.dat";
    std::string event_failed = "event_failed.dat";
    std::string event_passed = "event_passed.dat";
    asd.Evaluate(event_gen, event_failed, event_passed);
    asd.GetEfficiency();
    std::cout << "test" << std::endl;
    //return 0;
}
