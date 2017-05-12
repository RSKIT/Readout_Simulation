/*
    ROME (ReadOut Modelling Environment)
    Copyright © 2017  Rudolf Schimassek (rudolf.schimassek@kit.edu),
                      Felix Ehrler (felix.ehrler@kit.edu),
                      Karlsruhe Institute of Technology (KIT)
                                - ASIC and Detector Laboratory (ADL)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as 
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of the ROME simulation framework.
*/

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
