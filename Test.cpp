#include "bibliothek.h"
#include <gtest/gtest.h>

// Die Unit Tests können durch das Einfügen der Klassen kompiliert werden.
// g++ -std=c++20 Test.cpp bibliothek.cpp -lgtest -lgtest_main -pthread -o run_tests.exe
TEST(RechnerTests, countWords)
{
  EXPECT_EQ(2, Bibliothek::countWords("Hallo Welt!"));
  EXPECT_EQ(1, Bibliothek::countWords("Hallo-W"));
  EXPECT_EQ(2, Bibliothek::countWords("Wie geht\'s?"));
  EXPECT_EQ(2, Bibliothek::countWords("Wie,geht\'s?"));

  std::string textv = "Das ist ein Test.";
  std::unordered_set<std::string> stopwoerter = {"das", "ist", "ein", "der"};

  auto ergebnis = Bibliothek::worthaeufigkeit(textv, stopwoerter);

  for (const auto &[wort, anzahl] : ergebnis)
  {
    if (wort == "Test")
      EXPECT_EQ(1, anzahl);
    else if (wort == "das")
      EXPECT_EQ(0, anzahl);
  }

  std::string text = "  Hallo,\n\nWelt!";
  std::string html = "<h1>Hallo Welt</h1>p>";
  std::string markdown =
    "# Hallo Welt\n"
    "**Dies ist ein Test**";

  EXPECT_EQ(
    "Hallo Welt", Bibliothek::bereinigeText(text, TextFormat::PlainText));
  EXPECT_EQ("Hallo Welt", Bibliothek::bereinigeText(text, TextFormat::HTML));
  EXPECT_EQ(
    "Hallo Welt", Bibliothek::bereinigeText(text, TextFormat::Markdown));

  std::string textregex = "C++ ist leistungsfähig. Moderne C++-Programme.";
  auto treffer = Bibliothek::sucheMuster(textregex, "\\w\\s");
  for (const auto &t : treffer)
  {
    if (t.text == "t")
      EXPECT_EQ(6, t.position);
    if (t.text == "e")
      EXPECT_EQ(30, t.position);
  }

  std::string beispielText =
    "Wie lang sind die einzelnen Woerter?"
    "Wie lang sind die einzelnen Woerter?!";

  EXPECT_EQ(
    6, Bibliothek::analysiereText(beispielText).durchschnittlicheSatzlaenge);
  EXPECT_EQ(2, Bibliothek::analysiereText(beispielText).gesamtSaetze);

  std::string textCo = "Kuenstliche Intelligenz veraendert die Welt. ";

  auto ergebnisse = Bibliothek::analysiereKookkurrenz(textCo, 2);

  for (const auto &[paar, anzahl] : ergebnisse)
  {
    if (paar.first == "Intelligenz" && paar.second == "Kuenstliche")
      EXPECT_EQ(1, anzahl);
    if (paar.first == "Intelligenz" && paar.second == "veraendert")
      EXPECT_EQ(1, anzahl);
  }
  std::string beispielTextFRE = "Dies ist ein einfacher Beispieltext.";

  double score = Bibliothek::berechneFleschReadingEase(beispielTextFRE, false);

  EXPECT_EQ("49.480000", std::to_string(score));

  beispielTextFRE = "Das ist ein Satz. Das ist ein Beispiel!";

  score = Bibliothek::berechneFleschReadingEase(beispielTextFRE, false);

  EXPECT_EQ("107.600000", std::to_string(score));
}