// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include <Arduino.h>

// M5Stack default font can't render UTF-8 German umlauts correctly.
// Transliterate them to ASCII before printing.
inline String sanitizeGermanText(const String &input) {
    String out;
    out.reserve(input.length());
    for (size_t i = 0; i < input.length(); i++) {
        unsigned char c1 = input[i];
        if (c1 == 0xC3 && i + 1 < input.length()) {
            unsigned char c2 = input[i + 1];
            switch (c2) {
                case 0xA4: out += "ae"; i++; continue;
                case 0xB6: out += "oe"; i++; continue;
                case 0xBC: out += "ue"; i++; continue;
                case 0x84: out += "Ae"; i++; continue;
                case 0x96: out += "Oe"; i++; continue;
                case 0x9C: out += "Ue"; i++; continue;
                case 0x9F: out += "ss"; i++; continue;
                default: break;
            }
        }
        out += (char)c1;
    }
    return out;
}
