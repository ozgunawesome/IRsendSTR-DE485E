/*
 *   control sony STR-DE485E receiver with Toshiba TV remote. (Well it could be any remote.)
 */
#include "remote_codes.h"
#include "irsend_debug.h"

//#define SERIAL_PORT_ENABLE       1
//#define IR_RECV_DEBUG            1
//#define IR_SEND_DEBUG            1
//#define IR_PROC_DEBUG            1
//#define INPUT_SELECTOR_DEBUG     1
//#define DEBUG_ENABLE_ALL         1

#define SERIAL_BUFFER_LENGTH      128

// IR code read pin/write pin
#define PWM_IN_PIN                3
#define PWM_OUT_PIN               4

// V1-V2-V3-V4 selected video read start pin
// (4 consecutive bits will be read starting with V1 here)
#define INPUT_SELECTOR_V1_READ_PIN  6

// when the input value matches the selection here
// driver out pin will go HIGH (or LOW)
#define INPUT_SELECTOR_CHOICE     (VIDEO_1)
#define DRIVER_OUT_PIN            (A0)
#define DRIVER_OUT_INVERT         1
#define INPUT_SELECTOR_READ_TICKS 10

// repeat the IR code sent. Sony requires that the code 
// be sent at least 3 times with some delay in between
#define NUMBER_OF_REPEATS         3
#define DELAY_BETWEEN_REPEATS_MS  20

// IR send/receive feedback on arduino LED
#define IR_SEND_LED_FEEDBACK      false
#define IR_RECV_LED_FEEDBACK      true

// IRRemote library stuff
#define DECODE_NEC                1
#define EXCLUDE_EXOTIC_PROTOCOLS  1

// there's no IR LED the output is wired straight to the receiver's input
// USE_NO_SEND_PWM to simulate an active low IR receiver
// use open drain output to set the pin Hi-Z (receiver is 3.3v arduino is 5v)
#define USE_OPEN_DRAIN_OUTPUT     1
#define USE_NO_SEND_PWM           1
//#define USE_SOFT_SEND_PWM           1

#include <IRremote.h>

static uint32_t read_u32(const char *in)
{
  uint32_t out = 0;
  while (*in) {
    while (*in == ' ') in++;  // skip spaces
    if (!*in) break;
    out <<= 4;
    if (*in >= '0' && *in <= '9') out |= (*in - '0');
    if (*in >= 'A' && *in <= 'F') out |= (*in - 'A' + 10);
    if (*in >= 'a' && *in <= 'f') out |= (*in - 'a' + 10);
    in++;
  }
  return out;
}

#ifdef SERIAL_PORT_ENABLE
static int Serial_write(char c, FILE *) {
    if (c == '\n') Serial.write('\r');  // convert LF -> CRLF
    return Serial.write(c);
}
static FILE mystdout;
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  #ifdef INPUT_SELECTOR_V1_READ_PIN
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(INPUT_SELECTOR_V1_READ_PIN + i, INPUT);
  }
  #endif
  #if defined(DRIVER_OUT_PIN)
  pinMode(DRIVER_OUT_PIN, OUTPUT);
  #if defined(DRIVER_OUT_INVERT)
  digitalWrite(DRIVER_OUT_PIN, HIGH);
  #else
  digitalWrite(DRIVER_OUT_PIN, LOW);
  #endif
  #endif

  #ifdef SERIAL_PORT_ENABLE
  stdout = &mystdout;
  fdev_setup_stream(stdout, Serial_write, NULL, _FDEV_SETUP_WRITE);
  Serial.begin(115200);
  Serial.println();
  Serial.println("---------------------------------------------------------");
  Serial.println("  -- Ozzys remote translator for STR-DE485E receiver --  ");
  Serial.println("---------------------------------------------------------");
  Serial.println();
  #endif

  #if defined(USE_NO_SEND_PWM) || defined(USE_SOFT_SEND_PWM)
  IrSender.begin(PWM_OUT_PIN, IR_SEND_LED_FEEDBACK, LED_BUILTIN);
  #else
  IrSender.begin(IR_SEND_LED_FEEDBACK, LED_BUILTIN);
  #endif
  
  IrReceiver.begin(PWM_IN_PIN, IR_RECV_LED_FEEDBACK);
}

void sendcode(uint32_t code, uint8_t bits) {
  if (code != 0xffffffff && code >= 0b111111111111 && bits <= 12) {
    bits = 15;
  }  
  #if defined(SERIAL_PORT_ENABLE) && defined(IR_SEND_DEBUG)
  printf("Sending code : 0x%04X ", code);
  printf("over %u bits...\n", bits);
  #endif
  uint8_t numRepeats = NUMBER_OF_REPEATS;  
  IrReceiver.stop();
  while (true) {
    IrSender.sendSony(code, bits);
    delay(DELAY_BETWEEN_REPEATS_MS);
    if (--numRepeats == 0) break;
  }
  IrReceiver.start();
}

uint8_t nbits = DEFAULT_NUM_BITS;

#ifdef SERIAL_PORT_ENABLE
static char buffer[SERIAL_BUFFER_LENGTH];
static size_t length = 0;
int16_t lastcode = 0;
int16_t lastaddr = 1;

// gotumden kod uydurma modu
void discovery(int incr, int incaddr) {
  lastcode += incr;
  lastaddr += incaddr;
  uint32_t sending = ((uint32_t)lastcode << 5) | (lastaddr & 0b11111);
  sendcode(sending, nbits);
  printf("Gonderilen kod %04X ", sending);
  printf("cmd = %02X ", lastcode);
  printf("adr = %02X ", lastaddr);
  printf(" - format %02u bit - de(v)am / (t)ekrar / (g)eri \n", nbits);
}

void selftest() {
  printf("Self test mode enabled. reset to exit\n");
  while (true) {
     sendcode(BTN_VOLUME_UP, DEFAULT_NUM_BITS); 
     sendcode(BTN_VOLUME_DOWN, DEFAULT_NUM_BITS); 
  }
}
#endif

uint8_t btToggleValue = LOW;
void btToggle(uint8_t newValue) {
  if (newValue == btToggleValue) {
    return;
  }
  btToggleValue = newValue;
  
  #ifdef SERIAL_PORT_ENABLE
  Serial.print(F("bluetooth module enable pin setting to: "));
  Serial.println(newValue == HIGH ? "HIGH": "LOW");
  #endif
  
  #ifdef DRIVER_OUT_PIN
  #ifdef DRIVER_OUT_INVERT
  digitalWrite(DRIVER_OUT_PIN, !newValue);
  #else
  digitalWrite(DRIVER_OUT_PIN, newValue);
  #endif
  #endif
}

enum InputSelectorMode {
  VIDEO_1 = 0b0010,
  VIDEO_2 = 0b0011,
  DVD_LD  = 0b1011,
  ZERO    = 0b0000
};

#if defined(INPUT_SELECTOR_V1_READ_PIN)
InputSelectorMode currMode = 0, newMode;
void readAnalogValuesNoTimeout() {  
  newMode = ZERO;
  for (int i = 3; i >= 0; i--) {
    newMode |= (digitalRead(INPUT_SELECTOR_V1_READ_PIN + i) & 0x1) << i;
  }

  if (newMode != currMode) {
    #if defined(SERIAL_PORT_ENABLE) && defined(INPUT_SELECTOR_DEBUG)
    Serial.print("Input selector mode: "); Serial.print(newMode, BIN);
    Serial.print(" Previous mode: "); Serial.print(currMode, BIN);
    Serial.println();
    #endif
    
    currMode = newMode;
    
    #ifdef SERIAL_PORT_ENABLE
    Serial.print(F("Input selector mode changed: "));
    switch(newMode) {
      case VIDEO_1:        Serial.println(F("VIDEO 1")); break;
      case VIDEO_2:        Serial.println(F("VIDEO 2")); break;        
      case DVD_LD:         Serial.println(F("DVD/LD")); break;
      default:             Serial.println(newMode, BIN);
    }
    #endif    
    
    btToggle(currMode == INPUT_SELECTOR_CHOICE ? HIGH : LOW);
  }
}
#endif

#if defined(INPUT_SELECTOR_READ_TICKS)
uint16_t inputSelTimer = 0;
inline void readAnalogValues() {
  if (inputSelTimer++ < INPUT_SELECTOR_READ_TICKS) {
    return;
  }
  inputSelTimer = 0;
  readAnalogValuesNoTimeout();
}
#else
#define readAnalogValues() readAnalogValuesNoTimeout()
#endif

void loop() {
  #if defined(INPUT_SELECTOR_V1_READ_PIN)
  readAnalogValues();
  #endif
  
  uint32_t code = 0xFFFFFFFF;
  bool codeFound = false;

  #ifdef SERIAL_PORT_ENABLE
  if (Serial.available()) {
    const char c = Serial.read();

    if (c == '\r' || c == '\n') {
      buffer[length] = '\0';
      length = 0;
      #ifdef SERIAL_PORT_DEBUG
      printf("Switch on string: '%s'\n", buffer);
      #endif
      switch(*buffer) {
        case 'v':
          discovery(+1,0); break;
        case 'g':
          discovery(-1,0); break;
        case 't':
          discovery(0, 0); break;
        case '[':
          discovery(0,-1); break;
        case ']':
          discovery(0,+1); break;
        case 'l':
          btToggle(!btToggleValue); break;
        case 'q':
          selftest();
        case 'x': {
          if (nbits == 15) nbits = 12;
          else if (nbits == 12) nbits = 15;
          printf("new format %d bit.\n", nbits);
        } break;
        case 'w': {
          uint32_t newbits = read_u32(buffer + 1);
          if (newbits > 0 && newbits <= 20) {
            printf("New format entered %u bits.\n", newbits);
            nbits = newbits;
          } else {
            printf("entered %u bits make no sense\n", newbits);
          }
        } break;
        default:
          #ifdef SERIAL_PORT_DEBUG
          printf("Try read u32 from content: %s\n", buffer);
          #endif
          code = read_u32(buffer);
          if (code != 0) {
            printf("Manual Code Entered: %04X\n", code);
            codeFound = true;
          }
      }
    } else {
      #ifdef SERIAL_PORT_DEBUG
      printf("buffer[%u]", length);
      printf(" = %c\n", c);
      #endif
      buffer[length++] = c;
      if (length >= SERIAL_BUFFER_LENGTH) {
        #ifdef SERIAL_PORT_DEBUG
        printf("Buffer full, reset.\n");
        #endif
        length = 0;
      }
    }
  }
  #endif

  if (IrReceiver.decode()) {
    uint16_t const cmd = IrReceiver.decodedIRData.command, adr = IrReceiver.decodedIRData.address;
    
    #if defined(SERIAL_PORT_ENABLE) && defined(IR_RECV_DEBUG)
    printf("IR receiver received some shit! ");
    printf(" command = %04X   ", cmd); printf(" address = %04X \n", adr);
    #endif

    IrReceiver.resume(); // Enable receiving of the next value
    if (cmd == 0 && adr == 0) {
      return;
    }
    codeFound = true;    
    switch (adr) {
      case 0x40: // Toshiba tv kumandasi
      switch (cmd) {
        case 0x1A: code = BTN_VOLUME_UP;   break;
        case 0x1E: code = BTN_VOLUME_DOWN; break;
      }
      break;
      case 0x10: // teledunya yarrak kablotv kumandasi
      switch (cmd) {
        // power mower
        case 0x0A: code = BTN_POWER; break;    // POWER
        case 0x44: code = BTN_MUTING; break;   // MUTE

        case 0x4A: btToggle(!btToggleValue); break; // V_FORMAT
        case 0x24: code = BTN_SLEEP; break;         // SLEEP
        case 0x5E: break;                           // Teletext
        case 0x1E: code = BTN_TESTTONE; break;      // TV/Radio        

        // menu navigasyon
        case 0x00: code = BTN_UP; break;
        case 0x01: code = BTN_DOWN; break;
        case 0x02: code = BTN_RIGHT; break;
        case 0x03: code = BTN_LEFT; break;
        case 0x1F: code = KEY_ENTER; break;     // OK
        case 0x1A: code = BTN_AMP_MENU; break;  // MENU
        case 0x09: code = BTN_AMP_MENU; break;  // EXIT

        // ses modu secme
        case 0x05: code = BTN_MODE_UP; break;
        case 0x07: code = BTN_MODE_DOWN; break;

        // preset ses modlari
        case 0x46: code = BTN_2CH;                break;    //FAV
        case 0x0C: code = BTN_AUTO_DEC;           break;    //GUIDE
        case 0x04: code = BTN_ANALOG_DIRECT;      break;    //INFO
        case 0x1C: code = BTN_MPX_DUAL;           break;    //checkmark

        // volume control
        case 0x25: code = BTN_VOLUME_UP;          break;    // VOL+
        case 0x27: code = BTN_VOLUME_DOWN;        break;    // VOL-

        // program control
        case 0x21: code = BTN_CHANNEL_UP;          break;   // CH+
        case 0x22: code = BTN_CHANNEL_DOWN;        break;   // CH-

        // numpad (niyeyse?)
        case 0x11: code = KEY_1; break;
        case 0x12: code = KEY_2; break;
        case 0x13: code = KEY_3; break;
        case 0x14: code = KEY_4; break;
        case 0x15: code = KEY_5; break;
        case 0x16: code = KEY_6; break;
        case 0x17: code = KEY_7; break;
        case 0x18: code = KEY_8; break;
        case 0x19: code = KEY_9; break;
        case 0x10: code = KEY_0; break;
        case 0x43: break; // [*]
        case 0x1D: break; // [#]

        // source control
        case 0x4D: code = BTN_VIDEO1;     break;//RED
        case 0x0D: code = BTN_VIDEO2;     break;//GREEN
        case 0x0E: code = BTN_DVD;        break;//YELLOW
        case 0x0F: code = BTN_SA_CD;      break;//BLUE

        case 0x42: code = BTN_MD_TAPE;    break;//PAUSE
        case 0x4B: code = BTN_TUNER;      break;//ZOOM
      }
      break;
    }
    #if defined(SERIAL_PORT_ENABLE) && defined(IR_PROC_DEBUG)
    printf("Kumanda kodu alindi. Cihaz: %04X ", adr); printf("Tus: %04X ", cmd);
    if (codeFound) {
      if (code != 0xFFFFFFFF) {
        printf(" Karsilik gelen sinyal = 0x%04X\n", code);
      } else {   
        printf(" Bu tusa hicbir sik tanimlanmamis.\n"); 
      }
    }
    #endif
  }
  if (codeFound) {
    sendcode(code, nbits);
  }
}
