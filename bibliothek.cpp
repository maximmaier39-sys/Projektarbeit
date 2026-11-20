#include "bibliothek.h"


// Die Bibliothek kann als Klasse Bibliothek in jedem Projekt inkludiert werden.
// g++ -std=c++20 -O2 testprogram.cpp bibliothek.cpp -o testprogram.exe
bool Bibliothek::isWordChar(char c)
{
  auto uc = static_cast<unsigned char>(c);
  return std::isalnum(uc) || uc >= 128; //, locale("de_DE"));
}


// Liefert die Anzahl der Wörter im Text zurück.
// Eingabe Parameter ist der Text
// liefert Anzahl Wörter zurück
size_t Bibliothek::countWords(std::string_view text)
{
  size_t count = 0;
  bool in_word = false;

  for (size_t i = 0; i < text.length(); ++i)
  {
    char c = text[i];

    if (isWordChar(c))
    {
      // Wenn wir noch nicht in einem Wort waren, beginnt hier ein neues
      if (!in_word)
      {
        in_word = true;
        count++;
      }
    }
    else if (in_word && (c == '-' || c == '\''))
    {
      // Lookahead: Ein Bindestrich/Apostroph gehört nur zum Wort,
      // wenn direkt danach wieder ein Buchstabe folgt (z.B.
      // "State-of-the-Art")
      if (i + 1 < text.length() && isWordChar(text[i + 1]))
      {
        // Wir bleiben im Zustand "in_word"
        continue;
      }
      else
      {
        in_word = false;
      }
    }
    else
    {
      // Jedes andere Zeichen (Leerzeichen, Komma, Punkt, \n) beendet das
      // Wort
      in_word = false;
    }
  }

  return count;
}

// Berechnet die Worthäufigkeit aller Wörter außer den Stoppwörtern.
// Eingabe Parameter ist der Text und ein Sammlung mit Stoppwörtern,
// die bei der Häufigkeitsanalyse ausgeschlossen werden.
// Liefert die Häufigkeiten in einer unordered_map zurück.
std::unordered_map<std::string, int> Bibliothek::worthaeufigkeit(
    const std::string& text,
    const std::unordered_set<std::string>& stopwoerter )
{
    std::unordered_map<std::string, int> haeufigkeiten;
    std::string wort;
    std::cout << "Haeufigkeit der Woerter im Text: " << std::endl;
    for (char c : text)
    {
        // Nur die Buchstaben A-Z und a-z und Ziffern 0-9 werden in Wörtern akzeptiert.
        if (std::isalnum(static_cast<unsigned char>(c)))
        {
            wort += std::tolower(static_cast<unsigned char>(c));
        }
        else
        {
            if (!wort.empty())
            {
                if (stopwoerter.find(wort) == stopwoerter.end())
                {
                    haeufigkeiten[wort]++;
                }
                wort.clear();
            }
        }
    }

    // Letztes Wort verarbeiten (falls Text nicht mit Satzzeichen endet)
    if (!wort.empty() &&
        stopwoerter.find(wort) == stopwoerter.end())
    {
        haeufigkeiten[wort]++;
    }

    return haeufigkeiten;
}

// Alle html tags werden durch Leerzeichen ersetzet
// Eingabe Parameter ist der Text
// gibt den bereinigten Text zurück
std::string Bibliothek::entferneHTMLTags(const std::string& html)
{
    std::string result;
    bool inTag = false;
    bool spaceChar = false;

    for (char c : html)
    {
        if (c == '<')
        {
            inTag = true;
        }
        else if (c == '>')
        {
            inTag = false;
            spaceChar = true;
        }
        else if (!inTag)
        {
            if(spaceChar)
                result += " ";
            result += c;
            spaceChar = false;
        }
    }

    return result;
}

// Alle Markdown Zeichen im Text werden entfernt.
// Eingabe Parameter ist Text.
// Liefert den bereinigten Text zurück.
std::string Bibliothek::entferneMarkdown(const std::string& md)
{
    std::string result;

    for (size_t i = 0; i < md.size(); ++i)
    {
        char c = md[i];

        switch (c)
        {
            case '*':
            case '_':
            case '#':
            case '`':
            case '[':
            case ']':
            case '(':
            case ')':
            case '>':
                break;

            default:
                result += c;
        }
    }

    return result;
}

// Alle Satzzeichen werden durch Leerzeichen entfernt
// Eingabe Parameter ist Text.
// Liefert den bereinigten Text zurück.
std::string Bibliothek::normalisiereText(const std::string& text)
{
    std::string result;
    bool letztesLeerzeichen = false;

    for (char c : text)
    {
        if (std::isalnum(static_cast<unsigned char>(c)))
        {
            result += static_cast<unsigned char>(c);
            letztesLeerzeichen = false;
        }
        else
        {
            if (!letztesLeerzeichen && !result.empty())
            {
                result += ' ';
                letztesLeerzeichen = true;
            }
        }
    }

    if (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }

    return result;
}

// Auswahl Funktion für die Text Bereinigung
// Eingabe Parameter ist Text und Text Format
// Liefert den bereinigten Text
std::string Bibliothek::bereinigeText(
    const std::string& text,
    TextFormat format)
{
    std::string bereinigt;

    switch (format)
    {
        case TextFormat::PlainText:
            bereinigt = text;
            break;

        case TextFormat::HTML:
            bereinigt = entferneHTMLTags(text);
            break;

        case TextFormat::Markdown:
            bereinigt = entferneMarkdown(text);
            break;
    }

    return normalisiereText(bereinigt);
}

// Sucht ein Muster mit den regulären Ausrücken
// Eingabe Parameter ist Text und Suchmuster
// Liefert die Liste aller Suchergebnisse mit Position 
std::vector<Treffer> Bibliothek::sucheMuster(const std::string& text,
                                 const std::string& muster) {
    std::vector<Treffer> ergebnisse;

    std::regex regexMuster(muster);
    auto begin = std::sregex_iterator(text.begin(), text.end(), regexMuster);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        ergebnisse.push_back({
            it->str(),
            static_cast<size_t>(it->position())
        });
    }

    return ergebnisse;
}

// Analysiert den übergebenen Text und gibt die statistischen Werte zurück.
// Eingabe Parameter ist der Text.
// Liefert eine Struktur mit durchschnittlichen Wortlänge und Satzlänge, Wortverteilung, Anzahl Wörter und Sätze.
TextStatistik Bibliothek::analysiereText(const std::string& text) {
    TextStatistik stats;
    int aktuelleWortlaenge = 0;
    int woerterImAktuellenSatz = 0;
    long gesamtBuchstaben = 0;

    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        if (std::isalnum(c)) {
            aktuelleWortlaenge++;
        } else {
            // Wortende erreicht
            if (aktuelleWortlaenge > 0) {
                stats.gesamtWoerter++;
                woerterImAktuellenSatz++;
                gesamtBuchstaben += aktuelleWortlaenge;
                stats.wortlaengenVerteilung[aktuelleWortlaenge]++;
                aktuelleWortlaenge = 0;
            }

            // Satzende prüfen (inkl. Schutz vor Mehrfachzeichen wie "...")
            if (istSatzende(c)) {
                if (woerterImAktuellenSatz > 0) {
                    stats.gesamtSaetze++;
                    woerterImAktuellenSatz = 0;
                }
            }
        }
    }

    // Falls der Text ohne Leer- oder Satzzeichen endet
    if (aktuelleWortlaenge > 0) {
        stats.gesamtWoerter++;
        woerterImAktuellenSatz++;
        gesamtBuchstaben += aktuelleWortlaenge;
        stats.wortlaengenVerteilung[aktuelleWortlaenge]++;
    }

    // Falls der letzte Satz kein abschließendes Satzzeichen hat
    if (woerterImAktuellenSatz > 0) {
        stats.gesamtSaetze++;
    }

    // Durchschnitte berechnen
    if (stats.gesamtWoerter > 0) {
        stats.durchschnittlicheWortlaenge = 
            static_cast<double>(gesamtBuchstaben) / stats.gesamtWoerter;
    }

    if (stats.gesamtSaetze > 0) {
        stats.durchschnittlicheSatzlaenge = 
            static_cast<double>(stats.gesamtWoerter) / stats.gesamtSaetze;
    }

    return stats;
}

// Gibt die Ergebnisse übersichtlich in der Konsole aus
// Eingabeparameter Text Statistik
void Bibliothek::druckeStatistik(const TextStatistik& stats) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== Textanalyse-Ergebnisse ===\n";
    std::cout << "Gesamtanzahl Woerter:         " << stats.gesamtWoerter << "\n";
    std::cout << "Gesamtanzahl Saetze:          " << stats.gesamtSaetze << "\n";
    std::cout << "Durchschnittliche Wortlaenge: " << stats.durchschnittlicheWortlaenge << " Zeichen\n";
    std::cout << "Durchschnittliche Satzlaenge: " << stats.durchschnittlicheSatzlaenge << " Woerter\n\n";

    std::cout << "Verteilung der Wortlaengen:\n";
    for (const auto& [laenge, anzahl] : stats.wortlaengenVerteilung) {
        double prozent = (static_cast<double>(anzahl) / stats.gesamtWoerter) * 100.0;
        std::cout << "  " << std::setw(2) << laenge << " Zeichen: " 
                  << std::setw(3) << anzahl << "x (" << std::setw(5) << prozent << "%)\n";
    }
}

std::vector<std::string> Bibliothek::tokenize(const std::string& text) {
    std::string cleaned;
    for (unsigned char c : text) {
        if (std::isalnum(c)) {
            cleaned += static_cast<char>(c);
        } else {
            cleaned += ' ';
        }
    }

    std::vector<std::string> tokens;
    std::istringstream iss(cleaned);
    std::string word;
    while (iss >> word) {
        // Sehr kurze Wörter (z. B. Einzelbuchstaben) optional filtern
        if (word.length() > 1) {
            tokens.push_back(word);
        }
    }
    return tokens;
}

// Gibt alle Paare im Eingabefenster
// Eingabe Parameter ist Text und Fenster Größe
// Liefert eine Zuordnung von Wortpaaren und Häufigkeit.
std::map<Wortpaar, int> Bibliothek::analysiereKookkurrenz(
    const std::string& text,
    int fensterGroesse) {

    std::vector<std::string> woerter = tokenize(text); 
    std::map<Wortpaar, int> haeufigkeiten;

    for (size_t i = 0; i < woerter.size(); ++i) {

        for (size_t j = i + 1;
             j < woerter.size() && j <= i + fensterGroesse;
             ++j) {

            std::string wort1 = woerter[i];
            std::string wort2 = woerter[j];

            if (wort1 > wort2)
                std::swap(wort1, wort2);

            haeufigkeiten[{wort1, wort2}]++;
        }
    }

    return haeufigkeiten;
}

// Hauptfunktion zur Berechnung des Flesch-Reading-Ease-Scores
// Eingabe Parameter ist Text und Sprache Wahl
// Liefert den Flesch Reading Index 
double Bibliothek::berechneFleschReadingEase(const std::string& text, bool deutsch) {
    int saetze = 0;
    int woerter = 0;
    int silben = 0;

    // 1. Sätze zählen (anhand von Satzzeichen)
    for (char c : text) {
        if (c == '.' || c == '!' || c == '?') {
            saetze++;
        }
    }
    if (saetze == 0) saetze = 1; // Division durch Null verhindern

    // 2. Wörter und Silben zählen
    std::stringstream ss(text);
    std::string wort;
    while (ss >> wort) {
        // Prüfen, ob das Token überhaupt Buchstaben enthält
        bool hatBuchstaben = std::any_of(wort.begin(), wort.end(), [](unsigned char c) {
            return std::isalpha(c);
        });

        if (hatBuchstaben) {
            woerter++;
            silben += zaehleSilbenImWort(wort);
        }
    }

    if (woerter == 0) return 0.0;

    // Durchschnittswerte berechnen
    double asl = static_cast<double>(woerter) / saetze; // Average Sentence Length
    double asw = static_cast<double>(silben) / woerter; // Average Syllables per Word

    // 3. Formel anwenden
    if (deutsch) {
        // Deutsche Formel nach Toni Amstad
        return 180.0 - asl - (58.5 * asw);
    } else {
        // Englische Originalformel nach Rudolf Flesch
        return 206.835 - (1.015 * asl) - (84.6 * asw);
    }
}

// Hilfsfunktion: Zählt die Silben eines einzelnen Wortes (heuristisch)
int Bibliothek::zaehleSilbenImWort(const std::string& wort) {
    int silben = 0;
    bool vorherigerWarVokal = false;

    for (size_t i = 0; i < wort.length(); ++i) {
        // Ignoriere Satzzeichen am Anfang/Ende
        if (!std::isalpha(static_cast<unsigned char>(wort[i]))) {
            continue;
        }

        bool aktuellerIstVokal = istVokal(wort[i]);
        if (aktuellerIstVokal && !vorherigerWarVokal) {
            silben++;
        }
        vorherigerWarVokal = aktuellerIstVokal;
    }

    // Im Englischen ist ein 'e' am Wortende oft stumm (für deutsche Texte ggf. entfernen)
    if (wort.length() > 2 && std::tolower(wort.back()) == 'e' && !istVokal(wort[wort.length() - 2])) {
        if (silben > 1) silben--;
    }

    // Jedes Wort hat mindestens eine Silbe
    return std::max(1, silben);
}