#include <iostream>
#include <string>
#include <future>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include "bibliothek.h"

// Erstellen der Asyc Berechnung 
// g++ -std=c++20 -O2 async.cpp bibliothek.cpp -o testprogram.exe

class TextAnalyzer {
public:

    static std::future<std::unordered_map<std::string, int>> analyzeAsync(const std::string& text, const std::unordered_set<std::string>& stopwoerter ) {
        return std::async(std::launch::async, [text, stopwoerter]() {
            return Bibliothek::worthaeufigkeit(text, stopwoerter);
        });
    }
};

int main() {
    std::string largeText =
        "Dies ist ein großer Text, der analysiert werden soll...";

    std::unordered_set<std::string> stopwoerter =
    {
        "das", "ist", "ein", "der"
    };

    auto ergebnis = TextAnalyzer::analyzeAsync(largeText, stopwoerter);

    std::cout << "Analyse gestartet.\n";

    // Hier kann die GUI oder andere Logik weiterlaufen
    for (int i = 0; i < 10; ++i) {
        std::cout << "UI bleibt responsiv: " << i << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    }

    std::unordered_map<std::string, int> result = ergebnis.get();

     for (const auto& [wort, anzahl] : result)
    {
        std::cout << wort << ": " << anzahl << '\n';
    }

    return 0;
}