#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "fingerspelling.h"
#include "process_unicode.h"
#define IGNORE_MOD_TAP_INTERRUPT


#define S_LAYER 0
#define F_LAYER 1
#define M_LAYER 2


uint32_t f_chord = 0;                // Fingerspelling Chord
uint32_t m_chord = 0;                // Modifier chord

uint8_t current_mods = NOMODS;
uint8_t keydown_mods = NOMODS;
uint8_t keyup_mods = NOMODS;

uint8_t prev_layer = 0;


enum custom_keycodes {
PLACEHOLDER = SAFE_RANGE,  F_FN, F_PWR,
    F_SU, F_SD, F_TL, F_KL, F_PL,  F_WL, F_HL, F_RL, F_A, F_O,
    F_ST1, F_ST2, F_ST3, F_ST4, F_NL, F_NR,
    F_E, F_U, F_FR, F_RR, F_PR, F_BR, F_LR, F_GR, F_TR, F_SR, F_DR, F_ZR,
    F_NR_ON
    };


void SEND(uint32_t kc) {
    register_code(kc);
    unregister_code(kc);
}

uint8_t compare_and_set_mods (uint8_t prev, uint8_t next) {
  // SEND(KC_3);
  if ((prev & SUPER) > (next & SUPER))  { unregister_code(KC_LWIN); }
  if ((prev & SUPER) < (next & SUPER))  { register_code(KC_LWIN);   }
  if ((prev & ALT)   > (next & ALT))    { unregister_code(KC_LALT); }
  if ((prev & ALT)   < (next & ALT))    { register_code(KC_LALT);   }
  if ((prev & CTRL)  > (next & CTRL))   { unregister_code(KC_LCTL); }
  if ((prev & CTRL)  < (next & CTRL))   { register_code(KC_LCTL);   }
  // if ((prev & SHFT) == SHFT) {SEND(KC_4);}
  // if ((next & SHFT) == SHFT) {SEND(KC_5);}

  if ((prev & SHFT)  > (next & SHFT))   {unregister_code(KC_LSFT);
                                          // SEND(KC_6);
                                          }
  if ((prev & SHFT)  < (next & SHFT))   {
                                          // SEND(KC_7);
                                          register_code(KC_LSFT);   }
  return next;
}




void f_code(uint32_t kc, uint32_t bitmask) {
    if ((f_chord & bitmask) == bitmask) {
        SEND(kc);
        f_chord = f_chord & ~bitmask;
      }
}

void number_code(uint32_t kc, uint32_t bitmask) {
    if ((f_chord & bitmask) == bitmask) {
      SEND(kc);
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
  uint8_t starting_mods = current_mods;
  if (f_chord == (RNO | bitmask)) {
    compare_and_set_mods(starting_mods, mods);
    SEND(kc);
    compare_and_set_mods(mods, starting_mods);

    f_chord = f_chord & ~bitmask;
    }
}

void symbol_code_2(uint32_t kc1, uint32_t kc2, uint8_t mods1, uint8_t mods2, uint32_t bitmask) {
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
  // Right number bar is used for symbols

  if ((f_chord & RNO) > 0) {
    number_code(KC_1, LSU);
    number_code(KC_2, LFT);
    number_code(KC_3, LP);
    number_code(KC_4, LH);
    number_code(KC_5, LA);
    number_code(KC_5, ST1);
    number_code(KC_6, LSD);
    number_code(KC_7, LK);
    number_code(KC_8, LW);
    number_code(KC_9, LR);
    number_code(KC_0, ST2);
    number_code(KC_0, LO);

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
    symbol_code(KC_LBRC, SHFT, RNO | RL | RT);                      // {
    symbol_code(KC_GRV, NOMODS, RNO | RL | RS);                     // `
    symbol_code(KC_MINS, NOMODS, RNO | RL);                         // -


    // Starts with -G
    symbol_code(KC_RBRC, SHFT, RNO | RG | RS);                      // }
    symbol_code(KC_MINS, SHFT, RNO | RG);                           // _

    // Starts with -T
    symbol_code(KC_QUOT, CTRL | SHFT, RNO | RT | RS);               // cursive - wrap ""
    symbol_code(KC_COMM, SHFT, RNO | RT | RD);                      // <
    symbol_code(KC_QUOT, SHFT, RNO | RT);                           // "

    // Starts with -S
    symbol_code(KC_DOT, SHFT, RNO | RS | RZ);                       // >
    symbol_code(KC_QUOT, NOMODS, RNO | RS);                         // '

    // Starts with -D
    symbol_code_2(KC_MINS, KC_DOT, NOMODS, SHFT, RNO | RD);                  // ->

    // Starts with -Z
    symbol_code_3(KC_MINS, KC_DOT, KC_DOT, NOMODS, SHFT, SHFT, RNO | RZ);    // ->>

    f_chord = RNO;
  }
}

void number_chords(void){
  if ((f_chord & LNO) > 0) {
    number_code(KC_1, LSU);
    number_code(KC_2, LFT);
    number_code(KC_3, LP);
    number_code(KC_4, LH);
    number_code(KC_5, ST1);
    number_code(KC_5, ST1);
    number_code(KC_6, RF);
    number_code(KC_7, RP);
    number_code(KC_8, RL);
    number_code(KC_9, RT);
    number_code(KC_0, RD);
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

void move_to_m_chords(uint32_t bitmask) {
  m_chord = m_chord | bitmask;
  f_chord = f_chord & ~bitmask;
}



uint8_t mod_state(void) {
  uint8_t state = current_mods;

  if ((m_chord & (ST3 | RF)) == (ST3 | RF)) {
    state = state | SHFT;
  } else { state = state & ~SHFT;}

  return state;
}

void modifier_chords_pressed(uint32_t bitmask){
  m_chord = m_chord | bitmask;

  if ((m_chord & (ST3 | RF)) == (ST3 | RF)) {
      move_to_m_chords(RF | ST3);
  }
  if ((m_chord & (ST3 | RP)) == (ST3 | RP)) {
      move_to_m_chords(RF | ST3);
  }

  current_mods = compare_and_set_mods(current_mods, mod_state());
}

void modifier_chords_released(uint32_t bitmask){
  m_chord = m_chord & ~bitmask;
  current_mods = compare_and_set_mods(current_mods, mod_state());
}


bool process_fingerspelling(uint32_t bitmask, keyrecord_t *record) {
    if (record->event.pressed) {
      f_chord = f_chord | bitmask;
      modifier_chords_pressed(bitmask);
    }
    else {
      if ((f_chord & bitmask) > 0) {
        number_chords();
        symbol_chords();
        fingerspelling_chords();
      }
      modifier_chords_released(bitmask);
    }
    return true;
}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
   switch (keycode) {
            case KC_LSFT: if (record->event.pressed) { current_mods = current_mods | SHFT; }
                          else  { current_mods = current_mods & ~SHFT; };
                          return true;
            case F_FN: break;
            case F_PWR: break;
            case F_SU: return process_fingerspelling(LSU, record);
            case F_SD: return process_fingerspelling(LSD, record);
            case F_TL: return process_fingerspelling(LFT, record);
            case F_KL: return process_fingerspelling(LK,  record);
            case F_PL: return process_fingerspelling(LP,  record);
            case F_WL: return process_fingerspelling(LW,  record);
            case F_HL: return process_fingerspelling(LH,  record);
            case F_RL: return process_fingerspelling(LR,  record);
            case F_A: return process_fingerspelling(LA,  record);
            case F_O: return process_fingerspelling(LO,  record);

            case F_NL: if (record->event.pressed) { f_chord = f_chord | LNO; }
                       else  { process_fingerspelling(LNO, record);
                              f_chord = f_chord & ~LNO; };
                       return true;

            case F_NR_ON: if (record->event.pressed)  { prev_layer = S_LAYER;
                                                        layer_on(F_LAYER);
                                                        f_chord = f_chord | RNO;
                                                      }
                       return true;
            case F_NR: if (record->event.pressed) { prev_layer = 1;
                                                    f_chord = f_chord | RNO; }
                       else  { process_fingerspelling(RNO, record);
                               f_chord = f_chord & ~RNO;
                               if (prev_layer == 0) { layer_off(F_LAYER); }
                               };
                       return true;
            case F_ST1: return process_fingerspelling(ST1, record);
            case F_ST2: return process_fingerspelling(ST2, record);
            case F_ST3: return process_fingerspelling(ST3, record);
            case F_ST4: return process_fingerspelling(ST4, record);
            // case F_NL: return process_fingerspelling(LK,  record->event.pressed);
            // case F_NR: return process_fingerspelling(LK,  record->event.pressed);
            case F_E: return process_fingerspelling(RE, record);
            case F_U: return process_fingerspelling(RU, record);
            case F_FR: return process_fingerspelling(RF, record);
            case F_RR: return process_fingerspelling(RR, record);
            case F_PR: return process_fingerspelling(RP, record);
            case F_BR: return process_fingerspelling(RB, record);
            case F_LR: return process_fingerspelling(RL, record);
            case F_GR: return process_fingerspelling(RG, record);
            case F_TR: return process_fingerspelling(RT, record);
            case F_SR: return process_fingerspelling(RS, record);
            case F_DR: return process_fingerspelling(RD, record);
            case F_ZR: return process_fingerspelling(RZ, record);
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
                                   STN_A,   STN_O,   MO(M_LAYER),     F_NR_ON,      STN_E,   STN_U
    ),
    [F_LAYER] = LAYOUT_georgi(
    TO(S_LAYER), F_SU, F_TL, F_PL, F_HL, F_ST1,                       F_ST3, F_FR, F_PR, F_LR, F_TR, F_DR,
    TO(M_LAYER), F_SD, F_KL, F_WL, F_RL, F_ST2,                       F_ST4, F_RR, F_BR, F_GR, F_SR, F_ZR,
                             F_A,  F_O,  MO(M_LAYER),                 F_NR,  F_E,  F_U
    ),
    [M_LAYER] = LAYOUT_georgi(
    TO(S_LAYER), UC(0x2813), KC_NO, KC_NO, KC_NO, KC_NO,                   KC_PGUP,  KC_HOME, KC_UP,   KC_END,   KC_PGDN, KC_NO,
    KC_NO,       KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                   KC_NO,    KC_LEFT, KC_DOWN, KC_RIGHT, KC_NO,   KC_NO,
                               KC_NO, KC_NO, MO(M_LAYER),             KC_LCTRL, KC_LSFT, KC_NO
    )

};
