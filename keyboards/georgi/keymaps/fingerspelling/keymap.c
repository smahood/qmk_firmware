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
#include "keymap_fingerspelling.h"
#define IGNORE_MOD_TAP_INTERRUPT

enum custom_keycodes {
PLACEHOLDER = SAFE_RANGE,  F_FN, F_PWR,
    F_SU, F_SD, F_TL, F_KL, F_PL,  F_WL, F_HL, F_RL, F_A, F_O,
    F_ST1, F_ST2, F_ST3, F_ST4, F_NL, F_NR,
    F_E, F_U, F_FR, F_RR, F_PR, F_BR, F_LR, F_GR, F_TR, F_SR, F_DR, F_ZR};

uint32_t f_chord = 0;                // Fingerspelling Chord


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



void SEND_CTRL_SHIFTED(uint8_t kc) {
    bool curr_ctrled = (keyboard_report->mods & MOD_BIT(KC_LCTRL));
    bool curr_shifted = (keyboard_report->mods & MOD_BIT(KC_LSHIFT));

    if (!curr_ctrled) { register_code(KC_LCTRL); }
    if (!curr_shifted) { register_code(KC_LSFT); }

    SEND(kc);

    if (!curr_ctrled) { unregister_code(KC_LCTRL); }
    if (!curr_shifted) { unregister_code(KC_LSFT); }
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

void shifted_number_code(uint32_t kc, uint32_t bitmask) {
  if ((f_chord & bitmask) == bitmask) {
        register_code(KC_LSFT);
        SEND(kc);
        unregister_code(KC_LSFT);
        f_chord = f_chord & ~bitmask;
      }

}

void number_chords(void){
  if ((f_chord & RNO) > 0) {
    number_code(KC_0, RD);
    number_code(KC_9, RT);
    number_code(KC_8, RL);
    number_code(KC_7, RP);
    number_code(KC_6, RF);
    number_code(KC_5, ST1);
    number_code(KC_5, ST1);
    number_code(KC_4, LH);
    number_code(KC_3, LP);
    number_code(KC_2, LFT);
    number_code(KC_1, LSU);
    }
  else if ((f_chord & LNO) > 0) {
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

void symbol_chords(void){
  if ((f_chord & LNO) > 0) {
    shifted_number_code(KC_1, LSD);
    shifted_number_code(KC_2, LK);
    shifted_number_code(KC_3, LW);
    shifted_number_code(KC_4, LR);
    shifted_number_code(KC_5, ST2);
    shifted_number_code(KC_6, RR);
    shifted_number_code(KC_7, RB);
    shifted_number_code(KC_8, RG);
    shifted_number_code(KC_9, RS);
    shifted_number_code(KC_0, RZ);
    }
  }


void s_chords(void) {
    f_code(KC_Z, (LSU | LFT | LK | LP | LW));
    f_code(KC_J, LSU | LK | LW | LR);
    f_code(KC_V, LSU | LR);
    f_code(KC_SPC, LSU | RP);
    f_code(KC_S, LSU);
    f_code(KC_Z, (LSD | LFT | LK | LP | LW));
    f_code(KC_J, LSD | LK | LW | LR);
    f_code(KC_V, LSD | LR);
    f_code(KC_SPC, LSD | RP);
    f_code(KC_S, LSD);
}

void t_chords(void) {
    f_code(KC_G, LFT | LK | LP | LW);
    f_code(KC_N, LFT | LP | LH);
    f_code(KC_D, LFT | LK);
    f_code(KC_F, LFT | LP);
    f_code(KC_T, LFT);
}

void k_chords(void) {
    f_code(KC_Y, LK | LW | LR);
    f_code(KC_X, LK | LP);
    f_code(KC_Q, LK | LW);
    f_code(KC_C, LK | LR);
    f_code(KC_K, LK);
}

void p_chords(void) {
    f_code(KC_B, LP | LW);
    f_code(KC_M, LP | LH);
    f_code(KC_P, LP);
}

void h_chords(void) {
    f_code(KC_L, LH | LR);
    f_code(KC_H, LH);
}

void e_chords(void) {
    f_code(KC_I, RE | RU);
    f_code(KC_E, RE);
}

void p_right_chords(void) {
    f_code(KC_N, RP | RB);
    f_code(KC_P, RP);
}

void asterisk_chords(void){
  f_code(KC_BSPC, ST3);
}


bool process_fingerspelling(uint32_t bitmask, bool pressed) {
    if (pressed) {
        f_chord = f_chord | bitmask;
    } else if ((f_chord & bitmask) > 0) {
        number_chords();
        symbol_chords();
        asterisk_chords();
        s_chords();
        t_chords();
        k_chords();
        p_chords();
        f_code(KC_W, LW);
        h_chords();
        f_code(KC_H, LH);
        f_code(KC_R, LR);
        f_code(KC_A, LA);
        f_code(KC_O, LO);
        e_chords();
        f_code(KC_U, RU);
        f_code(KC_F, RF);
        f_code(KC_R, RR);
        p_right_chords();
        f_code(KC_B, RB);
        f_code(KC_L, RL);
        f_code(KC_G, RG);
        f_code(KC_T, RT);
        f_code(KC_S, RS);
        f_code(KC_D, RD);
        f_code(KC_Z, RZ);
    }
    return true;
}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
   switch (keycode) {
            case F_FN: break;
            case F_PWR: break;
            case F_SU: return process_fingerspelling(LSU, record->event.pressed);
            case F_SD: return process_fingerspelling(LSD, record->event.pressed);
            case F_TL: return process_fingerspelling(LFT, record->event.pressed);
            case F_KL: return process_fingerspelling(LK,  record->event.pressed);
            case F_PL: return process_fingerspelling(LP,  record->event.pressed);
            case F_WL: return process_fingerspelling(LW,  record->event.pressed);
            case F_HL: return process_fingerspelling(LH,  record->event.pressed);
            case F_RL: return process_fingerspelling(LR,  record->event.pressed);
            case F_A: return process_fingerspelling(LA,  record->event.pressed);
            case F_O: return process_fingerspelling(LO,  record->event.pressed);

            case F_NL: if (record->event.pressed) { f_chord = f_chord | LNO; }
                       else  { process_fingerspelling(LNO,  record->event.pressed);
                              f_chord = f_chord & ~LNO; };
                       return true;
            case F_NR: if (record->event.pressed) { f_chord = f_chord | RNO; }
                       else  { process_fingerspelling(RNO,  record->event.pressed);
                              f_chord = f_chord & ~RNO; };
                       return true;
            case F_ST1: return process_fingerspelling(ST1,  record->event.pressed);
            case F_ST2: return process_fingerspelling(ST2,  record->event.pressed);
            case F_ST3: return process_fingerspelling(ST3,  record->event.pressed);
            case F_ST4: return process_fingerspelling(ST4,  record->event.pressed);
            // case F_NL: return process_fingerspelling(LK,  record->event.pressed);
            // case F_NR: return process_fingerspelling(LK,  record->event.pressed);
            case F_E: return process_fingerspelling(RE,  record->event.pressed);
            case F_U: return process_fingerspelling(RU,  record->event.pressed);
            case F_FR: return process_fingerspelling(RF,  record->event.pressed);
            case F_RR: return process_fingerspelling(RR,  record->event.pressed);
            case F_PR: return process_fingerspelling(RP,  record->event.pressed);
            case F_BR: return process_fingerspelling(RB,  record->event.pressed);
            case F_LR: return process_fingerspelling(RL,  record->event.pressed);
            case F_GR: return process_fingerspelling(RG,  record->event.pressed);
            case F_TR: return process_fingerspelling(RT,  record->event.pressed);
            case F_SR: return process_fingerspelling(RS,  record->event.pressed);
            case F_DR: return process_fingerspelling(RD,  record->event.pressed);
            case F_ZR: return process_fingerspelling(RZ,  record->event.pressed);
        }
    return true;
}





void matrix_scan_user(void) {

};






// QMK Layers
#define STENO_LAYER   0
#define FINGERSPELLING_LAYER   1

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
    TO(FINGERSPELLING_LAYER), STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,       STN_ST3, STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
                              STN_PWR, STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2,       STN_ST4, STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
                              STN_A,   STN_O,  STN_N1,       STN_N7,  STN_E,   STN_U
    ),
    [FINGERSPELLING_LAYER] = LAYOUT_georgi(
    TO(STENO_LAYER), F_SU, F_TL, F_PL, F_HL, F_ST1,            F_ST3, F_FR, F_PR, F_LR, F_TR, F_DR,
    F_PWR,           F_SD, F_KL, F_WL, F_RL, F_ST2,            F_ST4, F_RR, F_BR, F_GR, F_SR, F_ZR,
                                        F_A, F_O,   F_NL,      F_NR,  F_E,  F_U
    )
};



    // P(LSU | LFT | LK | LP | LW,             SEND(KC_Z));
    // P(LSD | LFT | LK | LP | LW,             SEND(KC_Z));

    // P(LSU | LK | LW | LR,                   SEND(KC_J));
    // P(LSD | LK | LW | LR,                   SEND(KC_J));

    // P(LSU | LR,                             SEND(KC_V));
    // P(LSD | LR,                             SEND(KC_V));
    // P(LSU | RP,                             SEND(KC_SPC));
    // P(LSD | RP,                             SEND(KC_SPC));

    // P(LSU,                                  SEND(KC_S));
    // P(LSD,                                  SEND(KC_S));

    // P(LFT | LK | LP | LW,                   SEND(KC_G));
    // P(LFT | LP | LH,                        SEND(KC_N));
    // P(LFT | LK,                             SEND(KC_D));
    // P(LFT | LP,                             SEND(KC_F));
    // P(LFT,                                  SEND(KC_T));


    // P(LK | LW | LR,                         SEND(KC_Y));
    // P(LK | LP,                              SEND(KC_X));
    // P(LK | LW ,                             SEND(KC_Q));
    // P(LK | LR ,                             SEND(KC_C));



    // P(LP | LW ,                             SEND(KC_B));
    // P(LP | LH ,                             SEND(KC_M));
    // P(LP,                                   SEND(KC_P));

    // P(LW,                                   SEND(KC_W));

    // P(LH | LR ,                             SEND(KC_L));
    // P(LH,                                   SEND(KC_H));

    // P(LR,                                   SEND(KC_R));

    // P(LA,                                   SEND(KC_A));

    // P(LO,                                   SEND(KC_O));

    // P(RE | RU,                              SEND(KC_I));
    // P(RE,                                   SEND(KC_E));
    // P(RU,                                   SEND(KC_U));






    // P(LK,                  SEND(KC_K));




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
