#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class TextFormat
{
  PlainText,
  HTML,
  Markdown
};

struct Treffer
{
  std::string text;
  size_t position;
};

struct TextStatistik
{
  double durchschnittlicheWortlaenge = 0.0;
  double durchschnittlicheSatzlaenge = 0.0;
  std::map<int, int> wortlaengenVerteilung;
  int gesamtWoerter = 0;
  int gesamtSaetze = 0;
};

using Wortpaar = std::pair<std::string, std::string>;

class Bibliothek
{
private:
  static bool isWordChar(char c);
  static std::string normalisiereText(const std::string &text);
  static std::string entferneHTMLTags(const std::string &html);
  static std::string entferneMarkdown(const std::string &md);
  // Prüft, ob ein Zeichen ein Satzende markiert
  static bool istSatzende(char c) { return c == '.' || c == '!' || c == '?'; };
  static std::vector<std::string> tokenize(const std::string &text);
  static int zaehleSilbenImWort(const std::string &wort);

  static bool istVokal(char c)
  {
    c = std::tolower(static_cast<unsigned char>(c));
    std::string vokale = "aeiouyäöü";
    return vokale.find(c) != std::string::npos;
  }

public:
  // Liefert die Anzahl der Wörter im Text zurück.
  static size_t countWords(std::string_view text);
  static std::unordered_map<std::string, int> worthaeufigkeit(
    const std::string &text,
    const std::unordered_set<std::string> &stopwoerter = {});
  static std::string bereinigeText(const std::string &text, TextFormat format);
  static std::vector<Treffer>
  sucheMuster(const std::string &text, const std::string &muster);
  static TextStatistik analysiereText(const std::string &text);
  static void druckeStatistik(const TextStatistik &stats);
  static std::map<Wortpaar, int>
  analysiereKookkurrenz(const std::string &text, int fensterGroesse = 2);
  // Hauptfunktion zur Berechnung des Flesch-Reading-Ease-Scores
  static double
  berechneFleschReadingEase(const std::string &text, bool deutsch = true);
};