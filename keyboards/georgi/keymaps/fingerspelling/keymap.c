#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "fingerspelling.h"
#include "process_unicode.h"
#define IGNORE_MOD_TAP_INTERRUPT

#define S_LAYER 0                    // Steno layer
#define F_LAYER 1                    // Fingerspelling layer (includes numbers and symbols)
#define M_LAYER 2                    // Movement layer

uint32_t f_chord = 0;                // Fingerspelling Chord
uint32_t m_chord = 0;                // Modifier chord

uint8_t current_mods = NOMODS;       // Stores state of current steno modifiers - could be different than what QMK sees
uint8_t prev_layer = 0;              // Used to allow access to symbol layer from both steno and fingerspelling layers

const uint32_t LSFT_BITMASK   = ST1 | LH;
const uint32_t LCTRL_BITMASK  = ST1 | LP;
const uint32_t LALT_BITMASK   = ST1 | LFT;
const uint32_t LSUPER_BITMASK = ST1 | LSU;

const uint32_t RSFT_BITMASK   = ST3 | RF;
const uint32_t RCTRL_BITMASK  = ST3 | RP;
const uint32_t RALT_BITMASK   = ST3 | RL;
const uint32_t RSUPER_BITMASK = ST3 | RT;

enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE,  F_FN, F_PWR,
    F_SU, F_SD, F_TL, F_KL, F_PL,  F_WL, F_HL, F_RL, F_A, F_O,
    F_ST1, F_ST2, F_ST3, F_ST4,
    F_E, F_U, F_FR, F_RR, F_PR, F_BR, F_LR, F_GR, F_TR, F_SR, F_DR, F_ZR,
    F_NUM, F_NUM_ON
  };


void SEND(uint32_t kc) {
    register_code(kc);
    unregister_code(kc);
}

bool chord_match(uint32_t chord, uint32_t bitmask) {
  if ((chord & bitmask) == bitmask) { return true; }
  else { return false; }
}


uint8_t compare_and_set_mods (uint8_t prev, uint8_t next) {
  // Register and unregister modifier keys to move keyboard from prev to next state
  if ((prev & SHFT)  < (next & SHFT))   { register_code(KC_LSFT);   }
  if ((prev & SHFT)  > (next & SHFT))   { unregister_code(KC_LSFT); }

  if ((prev & CTRL)  < (next & CTRL))   { register_code(KC_LCTL);   }
  if ((prev & CTRL)  > (next & CTRL))   { unregister_code(KC_LCTL); }

  if ((prev & ALT)   < (next & ALT))    { register_code(KC_LALT);   }
  if ((prev & ALT)   > (next & ALT))    { unregister_code(KC_LALT); }

  if ((prev & SUPER) < (next & SUPER))  { register_code(KC_LWIN);   }
  if ((prev & SUPER) > (next & SUPER))  { unregister_code(KC_LWIN); }

  return next;
}

void extract_modifier_code(uint32_t bitmask) {
  if (chord_match(m_chord, bitmask)) { f_chord = f_chord & ~bitmask; }
}


void extract_modifier_chords(void) {
  // Checks and then moves matching chords from the fingerspelling chord to the modifier chords
  extract_modifier_code(LSFT_BITMASK);
  extract_modifier_code(LCTRL_BITMASK);
  extract_modifier_code(LALT_BITMASK);
  extract_modifier_code(LSUPER_BITMASK);
  extract_modifier_code(RSFT_BITMASK);
  extract_modifier_code(RCTRL_BITMASK);
  extract_modifier_code(RALT_BITMASK);
  extract_modifier_code(RSUPER_BITMASK);
}


uint8_t mod_code(uint32_t bitmask, uint8_t modifier, uint8_t state) {
  // Checks to see if the modifier state needs to change and returns the new modifier state.
  if (chord_match(m_chord, bitmask)) { state = state | modifier; }
  return state;
}


uint8_t mod_state(void) {
  // Returns the modifier state based on the current modifier chord
  uint8_t state = NOMODS;
  state = mod_code(LSFT_BITMASK, SHFT, state);
  state = mod_code(LCTRL_BITMASK, CTRL, state);
  state = mod_code(LALT_BITMASK, ALT, state);
  state = mod_code(LSUPER_BITMASK, SUPER, state);

  state = mod_code(RSFT_BITMASK, SHFT, state);
  state = mod_code(RCTRL_BITMASK, CTRL, state);
  state = mod_code(RALT_BITMASK, ALT, state);
  state = mod_code(RSUPER_BITMASK, SUPER, state);
  return state;
}


void f_code(uint32_t kc, uint32_t bitmask) {
  // Used for all normal keypresses in fingerspelling mode
  // Clears keys from chord once used
    if (chord_match(f_chord, bitmask)) {
        SEND(kc);
        f_chord = f_chord & ~bitmask;
      }
}


void number_code(uint32_t kc, uint32_t bitmask, bool twice) {
  // Type numbers, doesn't clear the number bar (can hold it down through multiple chords)
  // Must press the doubling key or reverse key again on each chord
  if ((f_chord & bitmask) == bitmask) {
    SEND(kc);
    if (twice == true) {
      SEND(kc);
    }
    f_chord = f_chord & ~bitmask;
  }
}


void unicode_code(uint32_t code, uint32_t bitmask) {
  // This is specific to Linux with IBus according to https://beta.docs.qmk.fm/features/feature_unicode#input-modes
  // There is a more standard way to output this but I couldn't get it to work
  if ((f_chord & bitmask) == bitmask) {
      register_code(KC_LCTL);
      register_code(KC_LSFT);
      SEND(KC_U);
      unregister_code(KC_LSFT);
      unregister_code(KC_LCTL);

      register_hex(code);

      SEND(KC_ENT);
      f_chord = f_chord & ~bitmask;
      }
}


void symbol_code(uint32_t kc, uint8_t mods, uint32_t bitmask) {
  // Send keycode with the desired modifiers, does not clear the number bar
  uint8_t starting_mods = current_mods;
  if (f_chord == (RNO | bitmask)) {
    compare_and_set_mods(starting_mods, mods);
    SEND(kc);
    compare_and_set_mods(mods, starting_mods);

    f_chord = f_chord & ~bitmask;
    }
}


void symbol_code_2(uint32_t kc1, uint32_t kc2, uint8_t mods1, uint8_t mods2, uint32_t bitmask) {
  // Same as symbol_code but can specify 2 keycodes and modifier settings
  uint8_t starting_mods = current_mods;
  if (f_chord == (RNO | bitmask)) {
    compare_and_set_mods(starting_mods, mods1);
    SEND(kc1);
    compare_and_set_mods(mods1, mods2);
    SEND(kc2);
    compare_and_set_mods(mods2, starting_mods);

    f_chord = f_chord & ~bitmask;
    }
}


void symbol_code_3(uint32_t kc1, uint32_t kc2, uint32_t kc3, uint8_t mods1, uint8_t mods2, uint8_t mods3, uint32_t bitmask) {
  // Same as symbol_code but can specify 3 keycodes and modifier settings
  uint8_t starting_mods = current_mods;
  if (f_chord == (RNO | bitmask)) {
    compare_and_set_mods(starting_mods, mods1);
    SEND(kc1);
    compare_and_set_mods(mods1, mods2);
    SEND(kc2);
    compare_and_set_mods(mods2, mods3);
    SEND(kc3);
    compare_and_set_mods(mods3, starting_mods);

    f_chord = f_chord & ~bitmask;
    }
}


void symbol_chords(void) {
  // Uses RNO for symbols and numbers
  // Number bar stays active at end of function, ready for next chord
  // Clears RZ and RD so they will have to be pressed again on next chord
  if ((f_chord & RNO) == RNO) {
    bool twice = ((f_chord & RZ) == RZ);      // Send each digit twice if RZ is pressed

    if ((f_chord & RD) == RD) {               // Reverse when RD is pressed
      number_code(KC_0, LO, twice);
      number_code(KC_0, ST2, twice);
      number_code(KC_9, LR, twice);
      number_code(KC_8, LW, twice);
      number_code(KC_7, LK, twice);
      number_code(KC_6, LSD, twice);
      number_code(KC_5, LA, twice);
      number_code(KC_5, ST1, twice);
      number_code(KC_4, LH, twice);
      number_code(KC_3, LP, twice);
      number_code(KC_2, LFT, twice);
      number_code(KC_1, LSU, twice);
      f_chord = f_chord &~ RD;
    }
    else {
      number_code(KC_1, LSU, twice);
      number_code(KC_2, LFT, twice);
      number_code(KC_3, LP, twice);
      number_code(KC_4, LH, twice);
      number_code(KC_5, LA, twice);
      number_code(KC_5, ST1, twice);
      number_code(KC_6, LSD, twice);
      number_code(KC_7, LK, twice);
      number_code(KC_8, LW, twice);
      number_code(KC_9, LR, twice);
      number_code(KC_0, ST2, twice);
      number_code(KC_0, LO, twice);
    }

    if (twice) { f_chord = f_chord &~ RZ; }

    // Starts with -F
    symbol_code(KC_9, CTRL | SHFT, RNO | RF | RR | RP | RB);        // cursive - wrap ()
    symbol_code(KC_EQL, SHFT, RNO | RF | RR);                       // +
    symbol_code(KC_7, SHFT, RNO | RF | RP | RL | RT);               // &
    symbol_code(KC_1, SHFT, RNO | RF | RP | RL);                    // !

    symbol_code(KC_9, SHFT, RNO | RF | RP );                        // (
    symbol_code_2(KC_DOT, KC_MINS, NOMODS, NOMODS, RNO | RF | RL);  // .-
    symbol_code(KC_DOT, NOMODS, RNO | RF);                          // .


    // Starts with -R
    symbol_code(KC_GRV, SHFT, RNO | RR | RP | RL | RT);             // ~
    symbol_code(KC_SLSH, SHFT, RNO | RR | RP | RL);                 // ?
    symbol_code(KC_6, SHFT, RNO | RR | RP | RG);                    // ^
    symbol_code(KC_SLSH, NOMODS, RNO | RR | RP);                    // /
    symbol_code(KC_4, SHFT, RNO | RR | RB | RG | RS);               // $
    symbol_code(KC_2, SHFT, RNO | RR | RB | RG);                    // @
    symbol_code(KC_0, SHFT, RNO | RR | RB);                         // )
    symbol_code(KC_COMM, NOMODS, RNO | RR);                         // ,

    // Starts with -P
    symbol_code(KC_LBRC, CTRL, RNO | RP | RB | RL | RG);            // cursive - wrap []
    symbol_code(KC_8, SHFT, RNO | RP | RB);                         // *
    symbol_code(KC_LBRC, NOMODS, RNO | RP | RL);                    // [
    symbol_code(KC_3, SHFT, RNO | RP | RL | RT);                    // #
    symbol_code(KC_BSLS, SHFT, RNO | RP | RG | RS);                 // |
    symbol_code(KC_BSLS, NOMODS, RNO | RP | RG);                    // backslash
    symbol_code(KC_SCLN, SHFT, RNO | RP);                           // :


    // Starts with -B
    symbol_code(KC_5, SHFT, RNO | RB | RG | RS);                    // %
    symbol_code(KC_RBRC, NOMODS, RNO | RB | RG);                    // ]
    symbol_code(KC_COMM, NOMODS, RNO | RB);                         // ,

    // Starts with -L
    symbol_code(KC_LBRC, CTRL | SHFT, RNO | RL | RG | RT | RS);     // cursive - wrap {}
    symbol_code(KC_EQL, NOMODS, RNO | RL | RG);                     // =
    symbol_code_2(KC_MINS, KC_DOT, NOMODS, SHFT, RNO | RL | RT | RD);  // ->
    symbol_code(KC_LBRC, SHFT, RNO | RL | RT);                      // {
    symbol_code(KC_GRV, NOMODS, RNO | RL | RS);                     // `
    symbol_code(KC_MINS, NOMODS, RNO | RL);                         // -



    // Starts with -G
    symbol_code_3(KC_MINS, KC_DOT, KC_DOT, NOMODS, SHFT, SHFT, RNO | RG | RS | RZ);    // ->>
    symbol_code(KC_RBRC, SHFT, RNO | RG | RS);                      // }
    symbol_code(KC_MINS, SHFT, RNO | RG);                           // _

    // Starts with -T
    symbol_code(KC_QUOT, CTRL | SHFT, RNO | RT | RS);               // cursive - wrap ""
    symbol_code(KC_COMM, SHFT, RNO | RT | RD);                      // <
    symbol_code(KC_QUOT, SHFT, RNO | RT);                           // "

    // Starts with -S
    symbol_code(KC_DOT, SHFT, RNO | RS | RZ);                       // >
    symbol_code(KC_QUOT, NOMODS, RNO | RS);                         // '

    f_chord = RNO;
  }
}


void fingerspelling_chords(void){
  // This fuction checks for all the standard fingerspelling rules in Steno Order
  // The chords will combine together based on non-conflicting rules
  // So `snowballs` could be stroked `SPHO/W-B/A-L/-LS`
  // The only active chord on the right side is `-PB` to write `n`

  // Start with S-
  f_code(KC_Z, (LSU | LFT | LK | LP | LW));
  f_code(KC_J, LSU | LK | LW | LR);
  f_code(KC_V, LSU | LR);
  f_code(KC_S, LSU);
  f_code(KC_Z, (LSD | LFT | LK | LP | LW));
  f_code(KC_J, LSD | LK | LW | LR);
  f_code(KC_V, LSD | LR);
  f_code(KC_S, LSD);

  // Start with T-
  f_code(KC_G, LFT | LK | LP | LW);
  f_code(KC_N, LFT | LP | LH);
  f_code(KC_D, LFT | LK);
  f_code(KC_F, LFT | LP);
  f_code(KC_T, LFT);

  // Start with K
  f_code(KC_Y, LK | LW | LR);
  f_code(KC_X, LK | LP);
  f_code(KC_Q, LK | LW);
  f_code(KC_C, LK | LR);
  f_code(KC_K, LK);

  // Start with P-
  f_code(KC_B, LP | LW);
  f_code(KC_M, LP | LH);
  f_code(KC_P, LP);

  // Start with W
  f_code(KC_W, LW);

  // Start with H
  f_code(KC_L, LH | LR);
  f_code(KC_H, LH);

  // Start with R-
  f_code(KC_R, LR);

  // Start with A
  f_code(KC_A, LA);

  // Start with O
  f_code(KC_O, LO);

  // Start with E
  f_code(KC_I, RE | RU);
  f_code(KC_E, RE);

  // Start with U
  f_code(KC_U, RU);

  // Start with F
  f_code(KC_F, RF);

  // Start with -R
  f_code(KC_R, RR);

  // Start with -P
  f_code(KC_N, RP | RB);
  f_code(KC_P, RP);

  // Start with B
  f_code(KC_B, RB);

  // Start with L
  f_code(KC_L, RL);

  // Start with G
  f_code(KC_G, RG);

  // Start with -T
  f_code(KC_T, RT);

  // Start with -S
  f_code(KC_S, RS);

  // Start with -D
  f_code(KC_D, RD);

  // Start with -Z
  f_code(KC_Z, RZ);
}


bool process_fingerspelling(uint32_t bitmask, keyrecord_t *record) {
    if (record->event.pressed) {
      // Add pressed key to fingerspelling chord and modifier chords
      // The modifier chord can have extra keys active without a problem
      f_chord = f_chord | bitmask;
      m_chord = m_chord | bitmask;

      // If the bitmasks in the fingerspelling match modifier chords,
      // clear them from the fingerspelling chord to prevent double processing.
      // These need to match those in mod_state
      extract_modifier_chords();

      // Activate the current modifier state
      current_mods = compare_and_set_mods(current_mods, mod_state());
    }
    else {
      // If the symbols are active, fingerspelling chords should not do anything
      // Can probably add an if statement here (left out in case I want to change something)
      symbol_chords();
      fingerspelling_chords();
      // Clear released key from the modifier chords
      m_chord = m_chord & ~bitmask;
      // Refresh the current modifiers after firing other chords
      // Expected behavior: When releasing modifier keys, fire all chords and change
      //                    modifier state. When releasing non-modifier keys, fire all
      //                    chords and leave modifier state the same.

      current_mods = compare_and_set_mods(current_mods, mod_state());
    }
    return true;
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // There might be a much nicer way to do this, but I haven't looked at changing it yet.
   switch (keycode) {
            // case KC_LSFT: if (record->event.pressed) { current_mods = current_mods | SHFT; }
            //               else  { current_mods = current_mods & ~SHFT; };
            //               return true;
            // case KC_LCTL: if (record->event.pressed) { current_mods = current_mods | CTRL; }
            //               else  { current_mods = current_mods & ~CTRL; };
            //               return true;
            // case KC_LALT: if (record->event.pressed) { current_mods = current_mods | ALT; }
            //               else  { current_mods = current_mods & ~ALT; };
            //               return true;
            // case KC_LWIN: if (record->event.pressed) { current_mods = current_mods | SUPER; }
            //               else  { current_mods = current_mods & ~SUPER; };
            //               return true;

            case F_NUM_ON: if (record->event.pressed)  { prev_layer = S_LAYER;
                                                        layer_on(F_LAYER);
                                                        f_chord = f_chord | RNO;
                                                      }
                            return true;

            case F_NUM: if (record->event.pressed) { prev_layer = 1;
                                                    f_chord = f_chord | RNO; }
                        else  { process_fingerspelling(RNO, record);
                               f_chord = f_chord & ~RNO;
                               if (prev_layer == 0) { layer_off(F_LAYER); }
                               };
                        return true;


            case F_SU:  return process_fingerspelling(LSU, record);
            case F_SD:  return process_fingerspelling(LSD, record);
            case F_TL:  return process_fingerspelling(LFT, record);
            case F_KL:  return process_fingerspelling(LK,  record);
            case F_PL:  return process_fingerspelling(LP,  record);
            case F_WL:  return process_fingerspelling(LW,  record);
            case F_HL:  return process_fingerspelling(LH,  record);
            case F_RL:  return process_fingerspelling(LR,  record);
            case F_A:   return process_fingerspelling(LA,  record);
            case F_O:   return process_fingerspelling(LO,  record);


            case F_ST1: return process_fingerspelling(ST1, record);
            case F_ST2: return process_fingerspelling(ST2, record);
            case F_ST3: return process_fingerspelling(ST3, record);
            case F_ST4: return process_fingerspelling(ST4, record);
            case F_E:   return process_fingerspelling(RE, record);
            case F_U:   return process_fingerspelling(RU, record);
            case F_FR:  return process_fingerspelling(RF, record);
            case F_RR:  return process_fingerspelling(RR, record);
            case F_PR:  return process_fingerspelling(RP, record);
            case F_BR:  return process_fingerspelling(RB, record);
            case F_LR:  return process_fingerspelling(RL, record);
            case F_GR:  return process_fingerspelling(RG, record);
            case F_TR:  return process_fingerspelling(RT, record);
            case F_SR:  return process_fingerspelling(RS, record);
            case F_DR:  return process_fingerspelling(RD, record);
            case F_ZR:  return process_fingerspelling(RZ, record);
        }
    return true;
}


void matrix_scan_user(void) {

};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Main layer, everything goes through here
    [S_LAYER] = LAYOUT_georgi(
    TO(F_LAYER), STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,         STN_ST3,      STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
    TO(M_LAYER), STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2,         STN_ST4,      STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
                                   STN_A,   STN_O,   MO(M_LAYER),     F_NUM_ON,      STN_E,   STN_U
    ),
    [F_LAYER] = LAYOUT_georgi(
    TO(S_LAYER), F_SU, F_TL, F_PL, F_HL, F_ST1,                       F_ST3, F_FR, F_PR, F_LR, F_TR, F_DR,
    TO(M_LAYER), F_SD, F_KL, F_WL, F_RL, F_ST2,                       F_ST4, F_RR, F_BR, F_GR, F_SR, F_ZR,
                             F_A,  F_O,  MO(M_LAYER),                 F_NUM,  F_E,  F_U
    ),
    [M_LAYER] = LAYOUT_georgi(
    TO(S_LAYER), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                   KC_PGUP,  KC_HOME, KC_UP,   KC_END,   KC_PGDN, KC_NO,
    TO(M_LAYER), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                   KC_NO,    KC_LEFT, KC_DOWN, KC_RIGHT, KC_NO,   KC_NO,
                               KC_NO, KC_NO, MO(M_LAYER),             KC_LCTRL, KC_LSFT, KC_LALT
    )

};
