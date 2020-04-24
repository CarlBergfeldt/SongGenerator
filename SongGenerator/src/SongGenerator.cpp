#pragma once

//#include "pch.h"
#include <iostream>
#include <string>

#include "Chord.h"

int main()
{

    auto isChord = [=](Note n, ChordQuality q, Chord c) {return n == c.GetRoot() && q == c.GetQuality(); };

    Chord chord = Chord(Note::C, ChordQuality::Major);

    bool isCMaj = isChord(Note::C, ChordQuality::Major, chord);    

    //const char * v = "Hej";

    std::cout << "Chord Progression: " << chord.ToString() << "\n";
}