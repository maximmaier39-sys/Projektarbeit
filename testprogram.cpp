#include "bibliothek.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Erstellen des Testprograms
// g++ -std=c++20 -O2 testprogram.cpp bibliothek.cpp -o testprogram.exe

int main()
{

  
  std::vector<std::pair<std::string, size_t>> test_cases = {
    {"Geht's dir gut? Es ist 14:30 Uhr.", 8}, // Apostroph, Zahlen & Uhrzeit
    {"... --- Hallo ... ???", 1},             // Reine Symbole
    {"Einfacher Satz mit fünf Wörtern.", 5},
    {"Hallo,Welt!Wie;geht.es?Dir?", 6}, // Keine Leerzeichen nach Satzzeichen
    {"Erste Zeile\nZweite Zeile\r\nDritte Zeile", 6}, // Verschiedene Umbrüche
    {"Das ist ein State-of-the-Art-System.", 4},      // Bindestrich-Wort
    {"   Viele   Leerzeichen   überall   ", 3},       // Whitespace-Rauschen
    {"Wort - noch ein Wort", 4}, // Isolierter Bindestrich zählt nicht
    {"", 0}                      // Leerer String
  };

  // Ausgabe der Anzahl Wörter
  for (const auto &[text, expected] : test_cases)
  {
    size_t result = Bibliothek::countWords(text);
    std::cout << "Ergebnis: " << result << " (Erwartet: " << expected << ")"
              << " -> " << (result == expected ? "OK" : "FEHLER") << "\n";
  }

  std::string textv =
    "Das ist ein Test. Dies ist nur ein Test. Der Test! Hallo hallo";

  std::unordered_set<std::string> stopwoerter = {"das", "ist", "ein", "der"};

  // Ausgabe der Worthäufigkeit
  auto ergebnis = Bibliothek::worthaeufigkeit(textv, stopwoerter);

  for (const auto &[wort, anzahl] : ergebnis)
  {
    std::cout << wort << ": " << anzahl << '\n';
  }

  // Bereinigung des Textes von Plain Text, Markdown und HTML Elemente.
  std::string text =
    "  Hallo,\n\nWelt!   Dies\tist ein   Test. "
    "Wie geht's?  ";

  std::string bereinigt =
    Bibliothek::bereinigeText(text, TextFormat::PlainText);

  std::cout << bereinigt << std::endl;

  std::string html = "<h1>Hallo Welt</h1><p>Dies ist ein Test.</p>";

  std::string markdown =
    "# Hallo Welt\n"
    "**Dies ist ein Test**";

  std::cout << Bibliothek::bereinigeText(html, TextFormat::HTML) << '\n';

  std::cout << Bibliothek::bereinigeText(markdown, TextFormat::Markdown)
            << '\n';

  std::string textregex =
    "C++ ist leistungsfähig. C++ unterstützt OOP. Moderne C++-Programme.";

  // Suche nach Suchmuster mit regulären Ausdrücken
  auto treffer = Bibliothek::sucheMuster(textregex, "\\w\\s");

  for (const auto &t : treffer)
  {
    std::cout << "Gefunden: \"" << t.text << "\" an Position " << t.position
              << '\n';
  }

  std::string beispielText =
    "... --- Hallo ... ???"
    "Wie lang sind die einzelnen Woerter und Saetze im Durchschnitt?";

  // Untersuche den Text für statistische Größen
  TextStatistik ergStat = Bibliothek::analysiereText(beispielText);

  Bibliothek::druckeStatistik(ergStat);

  std::string textCo =
    "Kuenstliche Intelligenz veraendert die Welt. "
    "Kuenstliche Intelligenz verbessert die Textanalyse. "
    "Moderne Textanalyse nutzt kuenstliche Intelligenz.";

  // Untersuche Wörter in dem Bereich des Abstandes Fenster Größe
  auto ergebnisse = Bibliothek::analysiereKookkurrenz(textCo, 2);

  std::cout << "Ko-Okkurrenzen:\n\n";

  for (const auto &[paar, anzahl] : ergebnisse)
  {
    std::cout << "(" << paar.first << ", " << paar.second << ") -> " << anzahl
              << '\n';
  }


  std::string beispielTextFRE =
    "Dies ist ein einfacher Beispieltext. Er dient dazu, die Lesbarkeit zu "
    "testen. Kurze Sätze erhöhen den Wert.";

  // Berechne den Index für die Text Komplexität
  double score = Bibliothek::berechneFleschReadingEase(beispielTextFRE, false);

  std::cout << "Flesch-Reading-Ease (Deutsch): " << score << std::endl;

  return 0;
}