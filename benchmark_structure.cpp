#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <chrono>

struct FastTextMetrics {
    size_t total_bytes = 0;
    size_t total_words = 0;
    size_t total_sentences = 0;
    double avg_word_length = 0.0;
    double median_word_length = 0.0;
    double avg_sentence_length = 0.0;
    size_t min_sentence_words = 0;
    size_t max_sentence_words = 0;
    
    // Index = Wortlänge, Wert = Anzahl (Array reicht bis Wortlänge 64 völlig aus)
    std::array<size_t, 64> word_length_dist{}; 
};

class FastTextAnalyzer {
private:
    static inline bool is_sentence_delimiter(char c) noexcept {
        return c == '.' || c == '!' || c == '?';
    }

    static inline bool is_word_delimiter(char c) noexcept {
        // Trennzeichen: Leerzeichen, Steuerzeichen und gängige Interpunktion
        return 
               c == ',' || c == ';' || c == ':' || 
               c == '"' || c == '\'' || c == '(' || c == ')' || 
               c == '[' || c == ']' || is_sentence_delimiter(c);
    }

public:
    // Null-Kopie: Nimmt string_view entgegen
    static FastTextMetrics analyze(std::string_view text) noexcept {
        FastTextMetrics m;
        m.total_bytes = text.size();
        if (text.empty()) return m;

        size_t current_word_len = 0;
        size_t current_sentence_words = 0;
        size_t total_char_count_in_words = 0;
        m.min_sentence_words = size_t(-1);

        auto finish_word = [&]() {
            if (current_word_len > 0) {
                m.total_words++;
                current_sentence_words++;
                total_char_count_in_words += current_word_len;
                
                // Im Histogramm erfassen (Werte > 63 werden gekappt)
                size_t capped_len = std::min(current_word_len, m.word_length_dist.size() - 1);
                m.word_length_dist[capped_len]++;
                
                current_word_len = 0;
            }
        };

        auto finish_sentence = [&]() {
            finish_word();
            if (current_sentence_words > 0) {
                m.total_sentences++;
                m.min_sentence_words = std::min(m.min_sentence_words, current_sentence_words);
                m.max_sentence_words = std::max(m.max_sentence_words, current_sentence_words);
                current_sentence_words = 0;
            }
        };


        // --- SINGLE PASS SCAN (Linearer Durchlauf) ---
        const char* p0 = text.data();
        for (const char* p = p0; p - p0  < text.size(); ++p) {

            char c = *p;
            if (c == ' ')
            {
                finish_word();
            }
            else if (isalnum(c))
             {
                // UTF-8 Zeichen zählen: Folgebytes (10xxxxxx) überspringen
                if ((static_cast<unsigned char>(c) & 0xC0) != 0x80) {
                    current_word_len++;
                }
            }
            else if (is_sentence_delimiter(c)) {
                finish_sentence();
            } 
            else {
                finish_word();
            }

        }
        
        // Letztes Wort/Satz abschließen (falls kein Punkt am Ende war)
        finish_sentence();

        if (m.total_words == 0) {
            m.min_sentence_words = 0;
            return m;
        }

        // --- SCHNELLE STATISTISCHE BERECHNUNGEN ---
        m.avg_word_length = static_cast<double>(total_char_count_in_words) / m.total_words;
        m.avg_sentence_length = static_cast<double>(m.total_words) / (m.total_sentences ? m.total_sentences : 1);

        // Median in O(1) Zeit berechnen direkt aus dem Histogramm (KEIN sortieren nötig!)
        size_t target1 = (m.total_words + 1) / 2;
        size_t target2 = (m.total_words + 2) / 2;
        size_t accumulated = 0;
        size_t med1 = 0, med2 = 0;

        for (size_t len = 1; len < m.word_length_dist.size(); ++len) {
            accumulated += m.word_length_dist[len];
            if (med1 == 0 && accumulated >= target1) med1 = len;
            if (med2 == 0 && accumulated >= target2) {
                med2 = len;
                break;
            }
        }
        m.median_word_length = (med1 + med2) / 2.0;

        return m;
    }
};

std::string generateText(std::size_t words)
{
    static constexpr const char* sample =
        "The quick brown fox jumps over the lazy dog. ";

    std::string text;
    text.reserve(words * 6);

    for (std::size_t i = 0; i < words; ++i)
    {
        text += sample;
    }

    return text;
}

void benchmark(std::size_t words)
{
    auto text = generateText(words);

    auto start = std::chrono::high_resolution_clock::now();

FastTextMetrics m = FastTextAnalyzer::analyze(text);

    // Zugriff auf Rohdaten ohne Umwege:
    double schnitt = m.avg_word_length;
    double median  = m.median_word_length;
    size_t woerter = m.total_words;

    // Häufigkeit einer bestimmten Wortlänge abfragen (z.B. Wörter mit 3 Buchstaben):
    size_t woerter_laenge_3 = m.word_length_dist[3];

    auto end = std::chrono::high_resolution_clock::now();

    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start)
            .count();

    double sizeMB =
        static_cast<double>(text.size()) /
        (1024.0 * 1024.0);

    double throughput =
        sizeMB / (ms / 1000.0);

    std::cout
        << std::setw(12) << words
        << " Wörter | "
        << std::setw(8) << sizeMB << " MB | "
        << std::setw(6) << ms << " ms | "
        << std::setw(8) << throughput << " MB/s | "
        //<< "Score: " << result.score
        << '\n';
}

int main()
{
    std::vector<std::size_t> sizes = {
        60000,
        300000
    };

    std::cout
        << "Wörter      | Größe | Zeit | Durchsatz\n"
        << "-------------------------------------------\n";

    for (auto size : sizes)
    {
        benchmark(size);
    }
}