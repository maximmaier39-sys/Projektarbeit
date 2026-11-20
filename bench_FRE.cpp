#include <benchmark/benchmark.h>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include "bibliothek.h"

// Der Benchmark kann mit Google Benchmark kompiliert werden.
//g++ -std=c++20 -O2 -fno-inline -gdwarf-4 -fno-omit-frame-pointer bench_test.cpp bibliothek.cpp -lbenchmark -lshlwapi -o bench.exe
// Zum Profiling wird das Tool sleepy verwendet.
// BENCHMARK_FILTER="BM_TokenizeLang/10000" 'C:\Program Files\Very Sleepy\sleepy.exe' -o profil.sleepy -r bench.exe
// sleepy.exe profil.sleepy

// Benchmark 1: Kurzer CSV-String
static void BM_TokenizeKurz(benchmark::State& state) {
    std::string input = "Das ist ein Satz.";

    for (auto _ : state) {
        auto result = Bibliothek::berechneFleschReadingEase(input);
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
        auto result = Bibliothek::berechneFleschReadingEase(input);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_TokenizeLang)->Arg(100)->Arg(1000)->Arg(10000);

BENCHMARK_MAIN();