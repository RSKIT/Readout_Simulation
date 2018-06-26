#
#   ROME (ReadOut Modelling Environment)
#   Copyright (c) 2017  Rudolf Schimassek (rudolf.schimassek@kit.edu),
#                       Felix Ehrler (felix.ehrler@kit.edu),
#                       Karlsruhe Institute of Technology (KIT)
#                                 - ASIC and Detector Laboratory (ADL)
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 3 as 
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#   This file is part of the ROME simulation framework.
#

import os.path

def countup(filename):
  
  if os.path.isfile("master.key"):
    buildnumber = open(filename,"r");
    text = buildnumber.read(16);
    id   = buildnumber.read(10);
    buildnumber.close();
    id   = int(id) + 1
    buildnumber = open(filename,"w");
    buildnumber.write(text+" " + str(id));
    buildnumber.close();

# Count up build ID before building ROME:
Command("build.cpp","", 
	[
	  countup("build.cpp"),
	])


SConscript('Main.scons', variant_dir = "build/", duplicate = 0)