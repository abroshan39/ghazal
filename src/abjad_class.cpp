/*
                  C++ class for the Abjad Calculator:
                          by Aboutaleb Roshan
             18 Tir, 1393 (11 Ramadan, 1435) (9 July, 2014)
                         ab.roshan39@gmail.com
*/

#include "abjad_class.h"

const wstring abjad::abjadCharacters = L"آاأإٱءبپجچدهةۀوؤزژحطیيئىﻯﻱکكگلمنسعفصقرشتثخذضظغ";
const wstring abjad::diacritics = L"\u064E\u0650\u064F\u0652\u064B\u064D\u064C\u0651\u0654\u0655\u0670\u0656\u0653\u0640";
const wchar_t abjad::zwnj = 0x200C;
const wchar_t abjad::zwj = 0x200D;

// DIACRITICS:
// A << E << O << SOKUN << AN << EN << ON << TASHDID << HAMZE << HAMZE_ZIR << ALEF_KHANJARI << ALEF_KHANJARI_ZIR << MADD << KESHIDE

void abjad::init(const wstring &str)
{
    abStr = str;
    value = 0;
    harf = 0;
    noghte = 0;
    calculated = false;
    pCHN = false;
}

int abjad::abChar(const wchar_t &ch, abjadType abt)
{
    int a = 0;

    switch(ch)
    {
    case L'آ':
    case L'ا':
    case L'أ':
    case L'إ':
    case L'ٱ':
    case L'ء': a = 1; break;
    case L'ب':
    case L'پ': a = 2; break;
    case L'ج':
    case L'چ': a = 3; break;
    case L'د': a = 4; break;
    case L'ه':
    case L'ة':
    case L'ۀ': a = 5; break;
    case L'و':
    case L'ؤ': a = 6; break;
    case L'ز':
    case L'ژ': a = 7; break;
    case L'ح': a = 8; break;
    case L'ط': a = 9; break;
    case L'ی':
    case L'ي':
    case L'ئ':
    case L'ى':
    case L'ﻯ':
    case L'ﻱ': a = 10; break;
    case L'ک':
    case L'ك':
    case L'گ': a = 20; break;
    case L'ل': a = 30; break;
    case L'م': a = 40; break;
    case L'ن': a = 50; break;
    case L'س': a = 60; break;
    case L'ع': a = 70; break;
    case L'ف': a = 80; break;
    case L'ص': a = 90; break;
    case L'ق': a = 100; break;
    case L'ر': a = 200; break;
    case L'ش': a = 300; break;
    case L'ت': a = 400; break;
    case L'ث': a = 500; break;
    case L'خ': a = 600; break;
    case L'ذ': a = 700; break;
    case L'ض': a = 800; break;
    case L'ظ': a = 900; break;
    case L'غ': a = 1000; break;
    default: return 0;
    }

    switch(abt)
    {
    case KABIR:
        return a;
    case SAGIR:
        if(ch == L'س' || ch == L'ش' || ch == L'خ' || ch == L'ظ')
            return 0;
        return a % 12;
    case VASIT:
        if(a <= 10)
            return a;
        else if(a > 10 && a <= 100)
            return (a / 10) + 9;
        else if(a > 100 && a <= 1000)
            return (a / 100) + 18;
        break;
    case MAKUS:
        if(a >= 1 && a <= 10)
            return (11 - a) * 100;
        else if(a >= 20 && a <= 100)
            return 110 - a;
        else if(a >= 200 && a <= 1000)
            return 11 - (a / 100);
        break;
    case SAGIR2:
        if(ch == L'ط' || ch == L'ص' || ch == L'ظ')
            return 0;
        return a % 9;
    }

    return 0;
}

int abjad::abCharMofassal(const wchar_t &ch, abjadType abt)
{
    wstring charName(L"");
    int c = 0;

    if(abt == SAGIR && (ch == L'س' || ch == L'ش' || ch == L'خ' || ch == L'ظ'))
        return 0;
    if(abt == SAGIR2 && (ch == L'ط' || ch == L'ص' || ch == L'ظ'))
        return 0;

    switch(ch)
    {
    case L'آ':
    case L'ا':
    case L'أ':
    case L'إ':
    case L'ٱ':
    case L'ء': charName = L"ألف"; break;
    case L'ب':
    case L'پ': charName = L"باء"; break;
    case L'ج':
    case L'چ': charName = L"جيم"; break;
    case L'د': charName = L"دال"; break;
    case L'ه':
    case L'ة':
    case L'ۀ': charName = L"هاء"; break;
    case L'و':
    case L'ؤ': charName = L"واو"; break;
    case L'ز':
    case L'ژ': charName = L"زاي"; break;
    case L'ح': charName = L"حاء"; break;
    case L'ط': charName = L"طاء"; break;
    case L'ی':
    case L'ي':
    case L'ئ':
    case L'ى':
    case L'ﻯ':
    case L'ﻱ': charName = L"ياء"; break;
    case L'ک':
    case L'ك':
    case L'گ': charName = L"كاف"; break;
    case L'ل': charName = L"لام"; break;
    case L'م': charName = L"ميم"; break;
    case L'ن': charName = L"نون"; break;
    case L'س': charName = L"سين"; break;
    case L'ع': charName = L"عين"; break;
    case L'ف': charName = L"فاء"; break;
    case L'ص': charName = L"صاد"; break;
    case L'ق': charName = L"قاف"; break;
    case L'ر': charName = L"راء"; break;
    case L'ش': charName = L"شين"; break;
    case L'ت': charName = L"تاء"; break;
    case L'ث': charName = L"ثاء"; break;
    case L'خ': charName = L"خاء"; break;
    case L'ذ': charName = L"ذال"; break;
    case L'ض': charName = L"ضاد"; break;
    case L'ظ': charName = L"ظاء"; break;
    case L'غ': charName = L"غين"; break;
    default: return 0;
    }

    for(unsigned int i = 0; i < charName.length(); i++)
        c += abChar(charName[i], abt);

    return c;
}

int abjad::abCharNoghte(const wchar_t &ch)
{
    switch(ch)
    {
    case L'ب': return 1;
    case L'پ': return 3;
    case L'ج': return 1;
    case L'چ': return 3;
    case L'ة': return 2;
    case L'ز': return 1;
    case L'ژ': return 3;
    case L'ی':
    case L'ي':
    case L'ئ':
    case L'ى':
    case L'ﻯ':
    case L'ﻱ': return 2;
    case L'ن': return 1;
    case L'ف': return 1;
    case L'ق': return 2;
    case L'ش': return 3;
    case L'ت': return 2;
    case L'ث': return 3;
    case L'خ': return 1;
    case L'ذ': return 1;
    case L'ض': return 1;
    case L'ظ': return 1;
    case L'غ': return 1;
    }

    return 0;
}

int abjad::abHesab(const wstring &str, abjadType abt, typeMojmMofa tmm)
{
    int c = 0;
    if(tmm == MOJMAL)
        for(unsigned int i = 0; i < str.length(); i++)
            c += abChar(str[i], abt);
    else
        for(unsigned int i = 0; i < str.length(); i++)
            c += abCharMofassal(str[i], abt);
    return c;
}

int abjad::abHesab(abjadType abt, typeMojmMofa tmm)
{
    value = 0;
    calculated = false;
    calculate(abStr, abt, tmm);
    return value;
}

int abjad::abHarf()
{
    calculate(abStr, KABIR, MOJMAL);
    return harf;
}

int abjad::abNoghte()
{
    calculate(abStr, KABIR, MOJMAL);
    return noghte;
}

void abjad::calculate(const wstring &str, abjadType abt, typeMojmMofa tmm)
{
    if(!calculated)
    {
        int tc = 0, strLen = str.length();
        if(tmm == MOJMAL)
        {
            for(int i = 0; i < strLen; i++)
            {
                value += abChar(str[i], abt);
                if(!pCHN)
                {
                    tc = abChar(str[i], KABIR);
                    if(tc != 0)
                        harf += 1;
                    noghte += abCharNoghte(str[i]);
                }
            }
        }
        else
        {
            for(int i = 0; i < strLen; i++)
            {
                value += abCharMofassal(str[i], abt);
                if(!pCHN)
                {
                    tc = abChar(str[i], KABIR);
                    if(tc != 0)
                        harf += 1;
                    noghte += abCharNoghte(str[i]);
                }
            }
        }
        calculated = true;
        pCHN = true;
    }
}
