#include <benchmark/benchmark.h>
#include <string>
#include <string_view>
#include <vector>

// Der Benchmark kann mit Google Benchmark kompiliert werden.
//g++ -std=c++20 -O2 -fno-inline -gdwarf-4 -fno-omit-frame-pointer bench.cpp bibliothek.cpp -lbenchmark -lshlwapi -o bench.exe
// Zum Profiling wird das Tool sleepy verwendet.
// BENCHMARK_FILTER="BM_TokenizeLang/10000" 'C:\Program Files\Very Sleepy\sleepy.exe' -o profil.sleepy -r bench.exe
// sleepy.exe profil.sleepy

bool isWordChar(char c) {
    auto uc = static_cast<unsigned char>(c);
    return std::isalnum(uc) || uc >= 128;//, locale("de_DE"));
}

__attribute__((noinline))
// Liefert die Anzahl der Wörter im Text zurück.   
size_t countWords(std::string_view text) {
    size_t count = 0;
    bool in_word = false;

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];

        if (isWordChar(c)) {
            // Wenn wir noch nicht in einem Wort waren, beginnt hier ein neues
            if (!in_word) {
                in_word = true;
                count++;
            }
        } 
        else if (in_word && (c == '-' || c == '\'')) {
            // Lookahead: Ein Bindestrich/Apostroph gehört nur zum Wort,
            // wenn direkt danach wieder ein Buchstabe folgt (z.B. "State-of-the-Art")
            if (i + 1 < text.length() && isWordChar(text[i + 1])) {
                // Wir bleiben im Zustand "in_word"
                continue; 
            } else {
                in_word = false;
            }
        } 
        else {
            // Jedes andere Zeichen (Leerzeichen, Komma, Punkt, \n) beendet das Wort
            in_word = false;
        }
    }

    return count;
}




// Benchmark 1: Kurzer CSV-String
static void BM_TokenizeKurz(benchmark::State& state) {
    std::string input = "Das ist ein Satz.";

    for (auto _ : state) {
        auto result = countWords(input);
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
        auto result = countWords(input);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_TokenizeLang)->Arg(100)->Arg(1000)->Arg(10000);

BENCHMARK_MAIN();