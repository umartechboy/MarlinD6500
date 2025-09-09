#include <Arduino.h>
//#include "..\..\pins\esp32\pins_D8500.h"

int Tone_STEP_PIN = -1;
int Tone_DIR_PIN = -1;
int Tone_ENABLE_PIN = -1;

extern void removePWMOnPin(const int16_t pin);
void InitMusic(int stepPin, int dirPin, int enablePin){
  Tone_STEP_PIN = stepPin;
  Tone_DIR_PIN = dirPin;
  Tone_ENABLE_PIN = enablePin;
}
long stepsForward = 0;
void Note(int note, float length, int amp = 6){  
  removePWMOnPin(Tone_STEP_PIN);
  removePWMOnPin(Tone_DIR_PIN);
  removePWMOnPin(Tone_ENABLE_PIN);
  pinMode(Tone_STEP_PIN, OUTPUT);
  pinMode(Tone_DIR_PIN, OUTPUT);
  pinMode(Tone_ENABLE_PIN, OUTPUT);

  digitalWrite(Tone_ENABLE_PIN, LOW); // Enable the motor

  // The motor is enabled. 
  // Lets create some sounds. We will do step in one direction and then the other for each pulse

  float freq = 440.0 * pow(2.0, (note - 69) / 12.0);
  float period = 1000000.0 / freq; // in microseconds
  // Length is in seconds
  float totalPulses = freq * length;
  for (long i = 0; i < totalPulses; i++){
    bool dir = stepsForward > 0;
    digitalWrite(Tone_DIR_PIN, dir ? HIGH:LOW);
    if (dir)
      stepsForward--;
    else
      stepsForward++;
    int pulseTime = 2;
    for (int j = 0; j < amp; j++){
        digitalWrite(Tone_STEP_PIN, 1);
        delayMicroseconds(pulseTime);
        digitalWrite(Tone_STEP_PIN, 0);
        delayMicroseconds(pulseTime);
    }
    if (period > 0)
      delayMicroseconds(period - 2 * amp * pulseTime);
  }
}
void TestNotes(){    
  // Test melody: C major scale, quarter-second each
  int scale[] = {60, 62, 64, 65, 67, 69, 71, 72}; // C4–C5
  for (int i = 0; i < 8; i++) {
    Note(scale[i], 0.25); // 0.25 s per note
    Serial.printf("Played note %d\n", scale[i]);
    delay(50);            // tiny pause between notes
  }
}
// ====== DEMO ======
const char* NOKIA_RTTTL =
  "Nokia:d=8,o=5,b=112:"
  "e6,16d#6,16f#6,16g#6,16c#6,16b5,16d#6,16e6,16b5,16c#6,16a5,"
  "p, e6,16d#6,16f#6,16g#6,16c#6,16b5,16d#6,16e6,16b5,16c#6,16a5";
  
// ====== RTTTL PLAYER ======
// Usage: playRTTTL("Nokia:d=8,o=5,b=112: e6,16f#6,8g#6,8a6, ...");
// ---------- existing helpers kept as-is ----------
struct RTTTLDefaults {
  int defDur;   // default duration denominator (e.g., 4 = quarter)
  int defOct;   // default octave (RTTTL: 4..7 typical; default 5)
  int bpm;      // beats per minute
  int amp;
};

static int noteNameToSemitone(char n, bool sharp) {
  switch (n) {
    case 'c': return 0  + (sharp?1:0);
    case 'd': return 2  + (sharp?1:0);
    case 'e': return 4;               // e# rarely used
    case 'f': return 5  + (sharp?1:0);
    case 'g': return 7  + (sharp?1:0);
    case 'a': return 9  + (sharp?1:0);
    case 'b': return 11;
  }
  return -1;
}

static int noteToMIDI(int semitoneFromC, int octaveRTTTL) {
  return (octaveRTTTL + 1) * 12 + semitoneFromC; // C-1 = 0
}

static float durToSeconds(int durDen, bool dotted, int bpm) {
  float whole_s = 4.0f * 60.0f / (float)bpm;
  float t = whole_s / (float)durDen;
  if (dotted) t *= 1.5f;
  return t;
}

void skipSpaces_(const char*& p) { while (*p==' '||*p=='\t'||*p=='\r'||*p=='\n') ++p; }

bool parseInt_(const char*& p, int& out) {
  skipSpaces_(p);
  if (*p<'0' || *p>'9') return false;
  int v=0; while (*p>='0' && *p<='9') { v = v*10 + (*p-'0'); ++p; }
  out = v; return true;
}

// ---------- ORIGINAL RTTTL (unchanged) ----------
void playRTTTL(const char* rtttl) {
  const char* p = rtttl;
  while (*p && *p != ':') ++p;
  if (*p != ':') return;
  ++p;

  RTTTLDefaults D = {8, 5, 120, 4};
  while (*p && *p != ':') {
    skipSpaces_(p);
    if (tolower(*p == 'd') && *(p+1) == '=') { p+=2; int v; if (parseInt_(p,v)) D.defDur=v; }
    else if (tolower(*p) == 'o' && *(p+1) == '=') { p+=2; int v; if (parseInt_(p,v)) D.defOct=v; }
    else if (tolower(*p) == 'b' && *(p+1) == '=') { p+=2; int v; if (parseInt_(p,v)) D.bpm=v; }
    else if (tolower(*p) == 'a' && *(p+1) == '=') { p+=2; int v; if (parseInt_(p,v)) D.amp=v; }
    if (D.amp < 1) D.amp = 1;
    if (D.amp > 8) D.amp = 8;
    while (*p && *p != ',' && *p != ':') ++p;
    if (*p == ',') ++p;
  }
  if (*p == ':') ++p;

  for (;;) {
    skipSpaces_(p);
    if (!*p) break;

    int thisDur = D.defDur, tmp;
    if (parseInt_(p, tmp)) thisDur = tmp;

    skipSpaces_(p);

    bool isPause = false;
    char n = tolower(*p);
    if (!n) break;
    if (n == 'p') { isPause = true; ++p; }
    else {
      if (n<'a' || n>'g') { while (*p && *p != ',') ++p; if (*p==',') ++p; continue; }
      ++p;
    }

    bool sharp=false;
    if (!isPause && *p=='#') { sharp=true; ++p; }

    bool dotted=false;
    if (*p=='.') { dotted=true; ++p; }

    int octave = D.defOct;
    if (*p>='0' && *p<='9') { octave = *p - '0'; ++p; }

    if (*p=='.') { dotted=true; ++p; }

    float seconds = durToSeconds(thisDur, dotted, D.bpm);

    if (isPause) {
      delay((unsigned long)(seconds*1000.0f + 0.5f));
    } else {
      int semi = noteNameToSemitone(n, sharp);
      if (semi >= 0) {
        int midi = noteToMIDI(semi, octave);
        Note(midi, seconds, D.amp);
      }
    }

    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;
  }
}

// ---------- NEW: NOKIA-COMPOSER STYLE ----------
/*
   Accepts input like:
   BPM: 60
   16e2 16d2 8#f 8#g 16#c2 16b 8d 8e 16b 16a 8#c 8e 2a 2-

   Differences handled:
   - tokens separated by spaces (or commas)
   - '#' may appear before OR after the note letter (#f == f#)
   - '-' means rest (e.g., 2-)
   - optional octave digit; if present, we apply 'octaveBias' (default +3) so "2" -> RTTTL 5
*/
void playComposer(const char* text, int octaveBias = 4, int defaultOct = 5) {
  const char* p = text;

  // Parse optional "BPM: N" line anywhere up front
  int bpm = 120;
  const char* q = p;
  skipSpaces_(q);
  if ((q[0]=='B' || q[0]=='b') && (q[1]=='P' || q[1]=='p') &&
      (q[2]=='M' || q[2]=='m')) {
    q += 3; skipSpaces_(q);
    if (*q==':' ) { ++q; }
    skipSpaces_(q);
    int v; if (parseInt_(q, v)) bpm = v;
    // move p to start of next line
    while (*q && *q!=':') ++q; p = q;
  }

  // Token loop
  while (true) {
    skipSpaces_(p);
    if (!*p) break;

    // read optional duration (defaults to 8 if absent)
    int dur = 8, tmp;
    if (parseInt_(p, tmp)) dur = tmp;

    skipSpaces_(p);

    // rest?
    if (*p == '-') {
      float seconds = durToSeconds(dur, false, bpm);
      delay((unsigned long)(seconds*1000.0f + 0.5f));
      ++p;
      continue;
    }

    // parse note + sharp
    bool sharp = false;
    char n = 0;

    if (*p == '#') { sharp = true; ++p; }            // # before letter (e.g., #f)
    if (*p >= 'A' && *p <= 'Z') n = *p + 32;         // tolower
    else if (*p >= 'a' && *p <= 'z') n = *p;
    if (n) ++p; else { // skip invalid token
      while (*p && *p!=' ' && *p!=',' && *p!=':' && *p!='\t') ++p;
      continue;
    }
    if (*p == '#') { sharp = true; ++p; }            // # after letter (e.g., f#)

    // optional octave
    int octave = defaultOct;
    if (*p >= '0' && *p <= '9') {
      int o = *p - '0'; ++p;
      octave = o + octaveBias;       // align Composer to RTTTL pitch range
    }

    // optional dotted (rare in this format; keep for completeness)
    bool dotted = false;
    if (*p == '.') { dotted = true; ++p; }

    // duration to seconds & play
    float seconds = durToSeconds(dur, dotted, bpm);

    int semi = noteNameToSemitone(n, sharp);
    if (semi >= 0) {
      int midi = noteToMIDI(semi, octave);
      Note(midi, seconds);
    }

    // advance to next token
    while (*p && *p != ' ' && *p != ',' && *p != '\n' && *p != '\t') ++p;
    if (*p == ',' ) ++p;
  }
}

// ---------- AUTO-DETECT WRAPPER ----------
void playAuto(const char* s) {
  // If it looks like RTTTL (has two ':' sections), use RTTTL
  int colons = 0; for (const char* p=s; *p; ++p) if (*p==':') ++colons;
  if (colons >= 2) { playRTTTL(s); return; }

  // If it begins with BPM: or bpm:, use Composer
  const char* p = s; skipSpaces_(p);
  if ((p[0]=='B'||p[0]=='b') && (p[1]=='P'||p[1]=='p') && (p[2]=='M'||p[2]=='m')) {
    playComposer(s); return;
  }

  // Otherwise, treat the whole thing as a line of Composer tokens (no header)
  playComposer(s);
}
#include "../../inc/MarlinConfig.h"
#include "../../gcode/gcode.h"

String monophonicHeader = "Mono:d=8,o=5,b=112:"; // default
void GcodeSuite::M35() {
  if (parser.string_arg[0]) {
    monophonicHeader = String(parser.string_arg);
    if (!monophonicHeader.endsWith(":"))
      monophonicHeader += ":";
    SERIAL_IMPL.printf("Monophonic header set to: %s\n", monophonicHeader.c_str());
  }
  else {
    SERIAL_IMPL.println("Monophonic header not changed");
  }
}
void GcodeSuite::M36() {
  String toPlay = monophonicHeader;
  if (parser.string_arg[0]) {
    toPlay += String(parser.string_arg);
    SERIAL_IMPL.printf("Monophonic tune: %s\n", toPlay.c_str());
    playAuto(toPlay.c_str());
  }
  else {
    SERIAL_IMPL.println("No tune data received");
  }
}
void GcodeSuite::M37() {
  SERIAL_IMPL.printf("Play: %s\n", parser.string_arg);
  playAuto(parser.string_arg);
}