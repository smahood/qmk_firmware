/*
 * Good on you for modifying your layout, this is the most nonQMK layout you will come across
 * There are three modes, Steno (the default), QWERTY (Toggleable) and a Momentary symbol layer
 *
 * Don't modify the steno layer directly, instead add chords using the keycodes and macros
 * from sten.h to the layout you want to modify.
 *
 * Observe the comment above processQWERTY!
 *
 * http://docs.gboards.ca
 */


#include QMK_KEYBOARD_H
#include "fingerspelling.h"
#define IGNORE_MOD_TAP_INTERRUPT

uint32_t f_chord = 0;                // Fingerspelling Chord
uint8_t current_mods = NOMODS;


enum custom_keycodes {
PLACEHOLDER = SAFE_RANGE,  F_FN, F_PWR,
    F_SU, F_SD, F_TL, F_KL, F_PL,  F_WL, F_HL, F_RL, F_A, F_O,
    F_ST1, F_ST2, F_ST3, F_ST4, F_NL, F_NR,
    F_E, F_U, F_FR, F_RR, F_PR, F_BR, F_LR, F_GR, F_TR, F_SR, F_DR, F_ZR};


void SEND(uint32_t kc) {
    register_code(kc);
    unregister_code(kc);
}


void SEND_SHIFTED(uint8_t kc) {
    bool curr_shifted = (keyboard_report->mods & MOD_BIT(KC_LSHIFT));
    if (curr_shifted) { SEND(kc); }
    else {  register_code(KC_LSFT);
            SEND(kc);
            unregister_code(KC_LSFT); }
}

void SEND_CTRLED(uint8_t kc) {
    bool curr_ctrled = (keyboard_report->mods & MOD_BIT(KC_LCTRL));
    if (curr_ctrled) { SEND(kc); }
    else {  register_code(KC_LCTRL);
            register_code(kc);
            unregister_code(kc);
            unregister_code(KC_LCTRL); }
}


uint8_t get_current_mods(void) {
    uint8_t mods = NOMODS;
    if (keyboard_report->mods & MOD_BIT(KC_LCTRL)) { mods = mods | CTRL; }
    if (keyboard_report->mods & MOD_BIT(KC_LSHIFT)) { mods = mods | SHFT; }
    return mods;
}


void compare_and_set_mods (uint8_t mods1, uint8_t mods2) {
        if ((mods1 & SUPER) > (mods2 & SUPER)) {unregister_code(KC_LWIN);}
        if ((mods1 & SUPER) < (mods2 & SUPER)) {register_code(KC_LWIN);}
        if ((mods1 & ALT) > (mods2 & ALT)) {unregister_code(KC_LALT);}
        if ((mods1 & ALT) < (mods2 & ALT)) {register_code(KC_LALT);}
        if ((mods1 & CTRL) > (mods2 & CTRL)) {unregister_code(KC_LCTL);}
        if ((mods1 & CTRL) < (mods2 & CTRL)) {register_code(KC_LCTL);}
        if ((mods1 & SHFT) > (mods2 & SHFT)) {unregister_code(KC_LSFT);}
        if ((mods1 & SHFT) < (mods2 & SHFT)) {register_code(KC_LSFT);}
}




void SEND_CTRL_SHIFTED(uint8_t kc) {
    bool curr_ctrled = (keyboard_report->mods & MOD_BIT(KC_LCTRL));
    bool curr_shifted = (keyboard_report->mods & MOD_BIT(KC_LSHIFT));

    if (!curr_ctrled) { register_code(KC_LCTRL); }
    if (!curr_shifted) { register_code(KC_LSFT); }

    SEND(kc);

    if (!curr_ctrled) { unregister_code(KC_LCTRL); }
    if (!curr_shifted) { unregister_code(KC_LSFT); }
}


void symbol_code(uint32_t kc, uint8_t mods, uint32_t bitmask) {
    uint8_t starting_mods = current_mods;
  if ((f_chord & bitmask) == bitmask) {
    compare_and_set_mods(starting_mods, mods);
    SEND(kc);
    compare_and_set_mods(mods, starting_mods);

    f_chord = 0;
    }
}

void symbol_code_2(uint32_t kc1, uint32_t kc2, uint8_t mods1, uint8_t mods2, uint32_t bitmask) {
  uint8_t starting_mods = current_mods;
  if ((f_chord & bitmask) == bitmask) {
    compare_and_set_mods(starting_mods, mods1);
    SEND(kc1);
    compare_and_set_mods(mods1, mods2);
    SEND(kc2);
    compare_and_set_mods(mods2, starting_mods);

    f_chord = 0;
    }
}


void symbol_code_3(uint32_t kc1, uint32_t kc2, uint32_t kc3, uint8_t mods1, uint8_t mods2, uint8_t mods3, uint32_t bitmask) {
  uint8_t starting_mods = current_mods;
  if ((f_chord & bitmask) == bitmask) {
    compare_and_set_mods(starting_mods, mods1);
    SEND(kc1);
    compare_and_set_mods(mods1, mods2);
    SEND(kc2);
    compare_and_set_mods(mods2, mods3);
    SEND(kc3);
    compare_and_set_mods(mods3, starting_mods);

    f_chord = 0;
    }
}


void symbol_chords(void) {
  // Uses right hand combinations that are rarely or never seen in English words

  uint32_t FPL = RF | RP | RL;
  uint32_t PLT = RP | RL | RT;
  uint32_t FPLT = RF | RP | RL | RT;
  uint32_t PLTD = RP | RL | RT | RD;

  // uint32_t RBG = RR | RB | RG;
  // uint32_t BGS = RB | RG | RS;
  // uint32_t RBGS = RR | RB | RG | RS;
  // uint32_t BGSZ = RB | RG | RS | RZ;

  if ((f_chord & FPLT) > 0) {
    symbol_code(KC_LBRC, CTRL | SHFT, FPLT | RE | RU);
    symbol_code(KC_LBRC, SHFT, FPLT | RE);
    symbol_code(KC_RBRC, SHFT, FPLT | RU);

  }

  if ((f_chord & FPL) > 0) {
    symbol_code(KC_9, CTRL | SHFT, FPL | RE | RU);
    symbol_code(KC_9, SHFT, FPL | RE);
    symbol_code(KC_0, SHFT, FPL | RU);
  }

  if ((f_chord & PLTD) > 0) {
    symbol_code_2(KC_MINS, KC_COMM, NOMODS, SHFT, PLTD | LFT | LP);
    symbol_code_2(KC_MINS, KC_DOT, NOMODS, SHFT, PLTD | LP | LH);


    symbol_code_2(KC_COMM, KC_DOT, SHFT, SHFT, PLTD | RE | RU);
    symbol_code(KC_COMM, SHFT, PLTD | RE);
    symbol_code(KC_DOT, SHFT, PLTD | RU);
  }

  if ((f_chord & PLT) > 0) {

    symbol_code(KC_LBRC, CTRL, PLT | RE | RU);
    symbol_code(KC_LBRC, NOMODS, PLT | RE);
    symbol_code(KC_RBRC, NOMODS, PLT | RU);
  }

}

void number_code(uint32_t kc, uint32_t bitmask) {
    if ((f_chord & bitmask) == bitmask) {
      SEND(kc);
      f_chord = f_chord & ~bitmask;
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



void f_code(uint32_t kc, uint32_t bitmask) {
    if ((f_chord & bitmask) == bitmask) {
        SEND(kc);
        f_chord = f_chord & ~bitmask;
      }
}


void asterisk_chords(void){
  f_code(KC_BSPC, ST3);
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
        f_chord = f_chord | bitmask;
    } else if ((f_chord & bitmask) > 0) {
      current_mods = get_current_mods();
      number_chords();
      symbol_chords();
      asterisk_chords();
      fingerspelling_chords();
    }
    return true;
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
   switch (keycode) {
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
            case F_NR: if (record->event.pressed) { f_chord = f_chord | RNO; }
                       else  { process_fingerspelling(RNO, record);
                              f_chord = f_chord & ~RNO; };
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






// QMK Layers
#define STENO_LAYER   0
#define F_LAYER   1

/* Keyboard Layout
 * ,---------------------------------.    ,------------------------------.
 * | FN  | LSU | LFT | LP | LH | ST1 |    | ST3 | RF | RP | RL | RT | RD |
 * |-----+-----+-----+----+----|-----|    |-----|----+----+----+----+----|
 * | PWR | LSD | LK  | LW | LR | ST2 |    | ST4 | RR | BB | RG | RS | RZ |
 * `---------------------------------'    `------------------------------'
 *                   ,---------------,    .---------------.
 *                   | LNO | LA | LO |    | RE | RU | RNO |
 *                   `---------------'    `---------------'
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Main layer, everything goes through here
    [STENO_LAYER] = LAYOUT_georgi(
    TO(F_LAYER), STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,       STN_ST3, STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
    STN_PWR,  STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2,       STN_ST4, STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
    STN_A,   STN_O,  STN_N1,       STN_N7,  STN_E,   STN_U
    ),
    [F_LAYER] = LAYOUT_georgi(
    TO(STENO_LAYER), F_SU, F_TL, F_PL, F_HL, F_ST1,            F_ST3,   F_FR, F_PR, F_LR, F_TR, F_DR,
    F_PWR,           F_SD, F_KL, F_WL, F_RL, F_ST2,            F_ST4,   F_RR, F_BR, F_GR, F_SR, F_ZR,
                                 F_A,  F_O,  F_NL,             KC_LSFT, F_E,  F_U
    )
};


    // P(RP | RL | RB | RG,   SEND_SHIFTED(KC_8));

    // P(RF | RP | RL | RT,   SEND_SHIFTED(KC_EQL));
    // P(RR | RB | RG | RS,   SEND(KC_EQL));

    // P(RF | RP | RG,        SEND_SHIFTED(KC_BSLS));
    // P(RR | RP | RL,        SEND_SHIFTED(KC_SLSH));
    // P(RR | RP | RG,        SEND_SHIFTED(KC_6));
    // P(RF | RP | RL,        SEND_SHIFTED(KC_3));
    // P(RR | RB | RG,        SEND_SHIFTED(KC_5));
    // P(RP | RL | RT,        SEND_SHIFTED(KC_1));
    // P(RB | RG | RS,        SEND_SHIFTED(KC_2));

    // P(RF | RL,             SEND(KC_MINS));
    // P(RR | RG,             SEND_SHIFTED(KC_MINS));
    // P(RF | RR,             SEND_CTRL_SHIFTED(KC_9));
    // P(RF | RP,             SEND_SHIFTED(KC_SCLN));
    // P(RF | RT,             SEND_SHIFTED(KC_7));
    // P(RR | RS,             SEND_SHIFTED (KC_4));
    // P(RR | RP,             SEND(KC_SLSH));
    // P(RR | RB,             SEND(KC_SCLN));
    // P(RP | RB,             SEND_CTRLED(KC_LBRC));
    // P(RP | RL,             SEND(KC_DOT));
    // P(RP | RG,             SEND(KC_BSLS));
    // P(RB | RG,             SEND(KC_COMM));
    // P(RL | RG,             SEND_CTRL_SHIFTED(KC_LBRC));
    // P(RL | RT,             SEND_SHIFTED(KC_QUOT));
    // P(RG | RS,             SEND(KC_QUOT));
    // P(RT | RD,             SEND_SHIFTED(KC_GRV));
    // P(RT | RS,             SEND_CTRL_SHIFTED(KC_QUOT));
    // P(RS | RZ,             SEND(KC_GRV));




    // P(ST1,                 SEND(KC_BSPC));
    // P(ST2,                 SEND(KC_BSPC));
    // P(ST3,                 SEND(KC_BSPC));
    // P(ST4,                 SEND(KC_BSPC));
    // P(RNO,                 SEND(KC_LSFT));
    // P(RF,                  SEND_SHIFTED(KC_9));
    // P(RR,                  SEND_SHIFTED(KC_0));
    // P(RP,                  SEND(KC_LBRC));
    // P(RB,                  SEND(KC_RBRC));
    // P(RL,                  SEND_SHIFTED(KC_LBRC));
    // P(RG,                  SEND_SHIFTED(KC_RBRC));
    // P(RT,                  SEND_SHIFTED(KC_COMM));
    // P(RS,                  SEND_SHIFTED(KC_DOT));
