![ROME Logo](https://github.com/RSKIT/Readout_Simulation/blob/master/Logo_6.png)

# ROME (ReadOut Modelling Environment)

This is a simulation tool for different readout architectures of synchronous Particle Detectors developed at Karlsruhe Insitute of Technology (KIT) by Rudolf Schimassek and Felix Ehrler in the ASIC and Detector Laboratory (ADL).

The simulation aims at synchronous detector architectures. For asynchronous detectors refer to Enrico Junior Schioppa's implementation developed for the MALTA sensor.

The configuration of the simulation is done via XML files. For this, Lee Thomason's XML parsing library tinyxml2 is used (see https://github.com/leethomason/tinyxml2).
The configurable parts include:
* the detector's buffer structure
* combinatory logic of hit information between pixels for generating group hits
* the detector's pixels' geometries
* the state machine used for the readout
* the parameters of the event generation including characteristic curves for dead time and time walk

For the characteristics of the deadtime and the time walk q-splines are used. For this, Tino Kluge's implementation (see https://github.com/ttk592/spline/) is used.

For the analysis of the data the Evaluation class (evaluation.h/.cpp) is provided intended for use in the ROOT data analysis framework. An example script if provided in test.cpp.

For structuring the output of the simulation, all files (except the spline outputs) can be (also or exclusively) written to a zip archive. For this, the miniz.c library is used with a C++ wrapper written by Thomas Fussel (see https://github.com/tfussell/miniz-cpp).

In order to use output of physics simulations, the data analysis framework ROOT is used to access the data. It is available at its [CERN Website](https://root.cern.ch).

This software is NOT released jet. It was tested successfully on Ubuntu 16.04LTS compiled with g++ 5.4.0 and ROOT 5.34/36 and there will be no support for this version. Some information is collected on the [Wiki page](https://github.com/RSKIT/Readout_Simulation/wiki).