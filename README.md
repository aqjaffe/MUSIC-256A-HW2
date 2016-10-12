# MUSIC-256A-HW2

This project turns a user text input into a sequence of FM sounds.

The text is divided into words by whitespace, then the carrier frequency, modulator frequency, and index are computed from the ASCII encoding of the first, second, and last character of the word respectively. Each word plays for a duration proportional to its length. There is also a flange effect applied to every sound, created using Faust.

This was a challenging project to develop. I never quite figured out how to properly change colors/fonts of the UI elements, so the interface is still very plain--although the program itself does not require much user interacton to run anyway. It also required integrating more C++ library functions (string, stringstream, vector) with JUCE, which proved very difficult at times.

There are lots of further directions I wished to take if I had more time: improving program graphics/aesthetics, adding a .txt file upload feature, adding more sound effects to the playback. Hopefully I'll improve this product later.

Created for HW1 of MUSIC 256A at Stanford CCRMA, created using JUCE and Faust.
