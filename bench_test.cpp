#include <benchmark/benchmark.h>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>

// Der Benchmark kann mit Google Benchmark kompiliert werden.
//g++ -std=c++20 -O2 -fno-inline -gdwarf-4 -fno-omit-frame-pointer bench_test.cpp bibliothek.cpp -lbenchmark -lshlwapi -o bench.exe
// Zum Profiling wird das Tool sleepy verwendet.
// BENCHMARK_FILTER="BM_TokenizeLang/10000" 'C:\Program Files\Very Sleepy\sleepy.exe' -o profil.sleepy -r bench.exe
// sleepy.exe profil.sleepy

struct ReadabilityResult {
    double score;
    std::string rating;
    size_t sentenceCount;
    size_t wordCount;
    size_t syllableCount;
    double avgSentenceLength;
    double avgSyllablesPerWord;
};

// Hilfsfunktion: Wandelt ASCII-Zeichen und gängige deutsche UTF-8 Umlaute in Kleinbuchstaben um
std::string toLowerGerman(std::string str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        if (c == 0xC3 && i + 1 < str.length()) { // UTF-8 Startbyte für Umlaute
            unsigned char next = str[i + 1];
            if (next == 0x84) { result += "\xc3\xa4"; i++; } // Ä -> ä
            else if (next == 0x96) { result += "\xc3\xb6"; i++; } // Ö -> ö
            else if (next == 0x9C) { result += "\xc3\xbc"; i++; } // Ü -> ü
            else { result += c; }
        } else {
            result += static_cast<char>(std::tolower(c));
        }
    }
    return result;
}

// Schätzt die Silbenanzahl eines deutschen Wortes
size_t countSyllablesGerman(std::string word) {
    word = toLowerGerman(word);

    // 1. Diphthonge ersetzen, da sie nur als EINE Silbe zählen
    std::vector<std::string> diphthongs = {
        "\xc3\xa4u", // äu
        "eu", "ei", "ey", "ai", "ay", "au", "ie", "oe", "ue", "ae"
    };

    for (const auto& d : diphthongs) {
        size_t pos = 0;
        while ((pos = word.find(d, pos)) != std::string::npos) {
            word.replace(pos, d.length(), "X"); // Ersetze durch Platzhalter
            pos += 1;
        }
    }

    // 2. Vokale und Platzhalter zählen
    size_t syllables = 0;
    for (size_t i = 0; i < word.length(); ++i) {
        char c = word[i];
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y' || c == 'X') {
            syllables++;
        } else if (static_cast<unsigned char>(c) == 0xC3 && i + 1 < word.length()) {
            unsigned char next = word[i + 1];
            // ä (\xc3\xa4), ö (\xc3\xb6), ü (\xc3\xbc)
            if (next == 0xA4 || next == 0xB6 || next == 0xBC) {
                syllables++;
                i++; // Byte überspringen
            }
        }
    }

    // Jedes Wort hat mindestens eine Silbe
    return std::max<size_t>(1, syllables);
}

// Berechnet den Flesch-Reading-Ease-Score (Amstad-Formel für Deutsch)
ReadabilityResult calculateFleschScore(const std::string& text) {
    size_t sentenceCount = 0;
    std::vector<std::string> words;
    std::string currentWord = "";
    bool inSentenceDelimiter = false;

    // Text parsen (Satzzeichen und Wörter identifizieren)
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = text[i];

        // Satz-Trennzeichen: . ! ?
        if (c == '.' || c == '!' || c == '?') {
            if (!inSentenceDelimiter) {
                sentenceCount++;
                inSentenceDelimiter = true;
            }
        } else if (!std::isspace(c)) {
            inSentenceDelimiter = false;
        }

        // Wort-Extraktion (inklusive UTF-8 Unterstützung)
        if (std::isalpha(c) || static_cast<unsigned char>(c) >= 0x80) {
            currentWord += c;
        } else {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        }
    }

    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }

    // Wenn der Text nicht mit einem Satzzeichen endet, aber Text da ist
    if (sentenceCount == 0 && !words.empty()) {
        sentenceCount = 1;
    }

    size_t wordCount = words.size();
    if (sentenceCount == 0 || wordCount == 0) {
        return {0.0, "Text zu kurz zur Analyse", 0, 0, 0, 0.0, 0.0};
    }

    // Silben zählen
    size_t totalSyllables = 0;
    for (const auto& w : words) {
        totalSyllables += countSyllablesGerman(w);
    }

    // Durchschnitte berechnen
    double asl = static_cast<double>(wordCount) / sentenceCount;        // Average Sentence Length
    double asw = static_cast<double>(totalSyllables) / wordCount;       // Average Syllables per Word

    // Formel nach Toni Amstad für deutsche Texte:
    // Flesch-DE = 180 - ASL - (58.5 * ASW)
    double score = 180.0 - asl - (58.5 * asw);

    // Qualitative Einstufung
    std::string rating;
    if (score >= 80.0)      rating = "Sehr leicht (Grundschule / Leichte Sprache)";
    else if (score >= 70.0) rating = "Leicht (Unterstufe / entspannte Webinhalte)";
    else if (score >= 60.0) rating = "Mittelschwer (Standardtext / gut verstaendlich)";
    else if (score >= 50.0) rating = "Maessig schwer (Gymnasialniveau / anspruchsvolle Artikel)";
    else if (score >= 30.0) rating = "Schwer (Fachliteratur / akademische Texte)";
    else                    rating = "Sehr schwer (Wissenschaftliche Texte / Beamtendeutsch)";

    return {score, rating, sentenceCount, wordCount, totalSyllables, asl, asw};
}

// Benchmark 1: Kurzer CSV-String
static void BM_TokenizeKurz(benchmark::State& state) {
    std::string input = "Das ist ein Satz.";

    for (auto _ : state) {
        auto result = calculateFleschScore(input);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_TokenizeKurz);

// Benchmark 2: Großer String (parametrisiert über state.range(0))
static void BM_TokenizeLang(benchmark::State& state) {
    const size_t anzahl_tokens = static_cast<size_t>(state.range(0));
    std::string input;
    input.reserve(anzahl_tokens * 8);

    for (size_t i = 0; i < anzahl_tokens; ++i) {
        //if (i > 0) input += ',';
        input += "Das ist ein Satz " + std::to_string(i) + ",";
    }

    for (auto _ : state) {
        auto result = calculateFleschScore(input);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_TokenizeLang)->Arg(100)->Arg(1000)->Arg(10000);

BENCHMARK_MAIN();