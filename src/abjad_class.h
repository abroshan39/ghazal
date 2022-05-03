/*
                  C++ class for the Abjad Calculator:
                          by Aboutaleb Roshan
             18 Tir, 1393 (11 Ramadan, 1435) (9 July, 2014)
                         ab.roshan39@gmail.com
*/

#ifndef ABJADCLASS_H
#define ABJADCLASS_H

#include <iostream>
#include <string.h>

using std::wstring;

class abjad
{
public:
    enum abjadType {KABIR, SAGIR, VASIT, MAKUS, SAGIR2};
    enum typeMojmMofa {MOJMAL, MOFASSAL};
    static const wstring abjadCharacters;
    static const wstring diacritics;
    static const wchar_t zwnj;
    static const wchar_t zwj;

public:
    abjad() {init();}
    abjad(const wstring &str) {init(str);}
    ~abjad() {;}

    static int abChar(const wchar_t &ch, abjadType = KABIR);
    static int abCharMofassal(const wchar_t &ch, abjadType = KABIR);
    static int abCharNoghte(const wchar_t &ch);
    static int abHesab(const wstring &str, abjadType = KABIR, typeMojmMofa = MOJMAL);
    int abHesab(abjadType = KABIR, typeMojmMofa = MOJMAL);
    int abHarf();
    int abNoghte();

    wstring assign(const wstring &str) {init(str); return abStr;}
    wstring text() {return abStr;}
    void clear() {init();}
    bool empty() const {return abStr.empty();}
    size_t size() const {return abStr.size();}
    size_t length() const {return abStr.length();}
    void operator=(const wstring &str) {init(str);}

private:
    void init(const wstring &str = L"");
    void calculate(const wstring &str, abjadType, typeMojmMofa);

private:
    wstring abStr;
    int value, harf, noghte;
    bool calculated, pCHN;
};
#endif // ABJADCLASS_H
