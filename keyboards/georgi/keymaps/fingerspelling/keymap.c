#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "fingerspelling.h"
#include "process_unicode.h"
#define IGNORE_MOD_TAP_INTERRUPT

#define S_LAYER 1                    // Steno layer
#define F_LAYER 0                    // Fingerspelling layer (includes numbers and symbols)

uint32_t chord   = 0;                // Full Chord
uint32_t m_chord = 0;                // Modifier chord
uint32_t i_chord = 0;                // Instant chord
uint32_t f_chord = 0;                // Fingerspelling Chord
uint32_t plover_chord = 0;           // Store steno keypresses to prevent plover from outputting in edge cases

uint8_t current_mods = NOMODS;       // Stores state of current steno modifiers - could be different than what QMK sees
uint8_t prev_layer = 0;              // Used to allow access to symbol layer from both steno and fingerspelling layers

// bool interrupt_steno = false;        // Used to prevent plover from outputting anything in edge cases

const uint32_t instant_bitmask         = LNO;
const uint32_t symbol_bitmask          = RNO;   // Symbols and numbers share same layer
const uint32_t number_bitmask          = RNO;   // Can probably be separated, I haven't tried yet
const uint32_t number_reverse_bitmask  = RD;
const uint32_t number_doubling_bitmask = RZ;

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
    F_RNO, F_LNO
  };


  //////////////////////////////
 //     Utility Functions    //
//////////////////////////////


void SEND(uint32_t kc) {
    register_code(kc);
    unregister_code(kc);
}


bool chord_match(uint32_t chord, uint32_t bitmask) {
  if ((chord & bitmask) == bitmask) { return true; }
  else { return false; }
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


void swap_layer(uint8_t prev, uint8_t next) {
  if (prev != next) {
    prev_layer = prev;
    layer_on(next);
    layer_off(prev);
  }
}


  //////////////////////////////
 //         Modifiers        //
//////////////////////////////

void extract_modifier_code(uint32_t bitmask) {
  if (chord_match(m_chord, bitmask)) { f_chord = f_chord & ~bitmask; }
}


void extract_modifier_chords(void) {
  // Checks and then moves matching chords from the fingerspelling chord to the modifier chords
  extract_modifier_code(ST1 | LH);
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
  state = mod_code(ST1 | LH, SHFT, state);
  state = mod_code(LCTRL_BITMASK, CTRL, state);
  state = mod_code(LALT_BITMASK, ALT, state);
  state = mod_code(LSUPER_BITMASK, SUPER, state);

  state = mod_code(RSFT_BITMASK, SHFT, state);
  state = mod_code(RCTRL_BITMASK, CTRL, state);
  state = mod_code(RALT_BITMASK, ALT, state);
  state = mod_code(RSUPER_BITMASK, SUPER, state);
  return state;
}


  //////////////////////////////
 //      Instant Chords      //
//////////////////////////////

void instant_code(uint32_t kc, uint8_t mods, uint32_t bitmask, keyrecord_t *record) {
  uint8_t starting_mods = current_mods;
  if (chord_match(i_chord, bitmask)) {
    compare_and_set_mods(starting_mods, mods);

    if (record->event.pressed){
      f_chord = f_chord & ~bitmask & ~instant_bitmask;
      register_code(kc);
    }
    else {
      unregister_code(kc);
      i_chord = i_chord & ~bitmask;
    }
    compare_and_set_mods(mods, starting_mods);
    }
}


void instant_chords(uint32_t bitmask, keyrecord_t *record) {
  // These register on keydown, unregister on keyup
  // Useful for keys that you can hold down - arrows, space, enter etc.
  
  instant_code(KC_ESC, NOMODS, LFT | LP, record); 
  instant_code(KC_DEL, NOMODS, LK | LP, record); 
  instant_code(KC_BSPC, NOMODS, LK | LW, record); 
  
  instant_code(KC_ENT, NOMODS, LP | LH, record); 
  instant_code(KC_TAB, NOMODS, LP | LR, record); 
  instant_code(KC_SPC, NOMODS, LW | LR, record); 

  instant_code(KC_HOME, NOMODS, RF, record);
  instant_code(KC_LEFT, NOMODS, RR, record);
  instant_code(KC_UP, NOMODS, RP, record);
  instant_code(KC_DOWN, NOMODS, RB, record);
  instant_code(KC_END, NOMODS, RL, record);
  instant_code(KC_RIGHT, NOMODS, RG, record);

}


bool process_instant(uint32_t bitmask, keyrecord_t *record) {
    if (record->event.pressed) {
      i_chord |= bitmask;
      if(chord_match(i_chord,  instant_bitmask)){
        instant_chords(bitmask, record);
      }
    }
    else {
      if(chord_match(i_chord,  instant_bitmask)){
        instant_chords(bitmask, record);
      }
    }
    return true;
}


  //////////////////////////////
 //         Symbols          //
//////////////////////////////

void symbol_code(uint32_t kc, uint8_t mods, uint32_t bitmask) {
  // Send keycode with the desired modifiers, does not clear the number bar
  uint8_t starting_mods = current_mods;
  if (chord_match(f_chord, symbol_bitmask | bitmask)) {
    compare_and_set_mods(starting_mods, mods);
    SEND(kc);
    compare_and_set_mods(mods, starting_mods);

    f_chord = f_chord & ~bitmask;
    }
}


void symbol_code_2(uint32_t kc1, uint32_t kc2, uint8_t mods1, uint8_t mods2, uint32_t bitmask) {
  // Same as symbol_code but can specify 2 keycodes and modifier settings
  uint8_t starting_mods = current_mods;
  if (chord_match(f_chord, symbol_bitmask | bitmask)) {
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
  if (chord_match(f_chord, symbol_bitmask | bitmask)) {
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
  // Starts with -F
  symbol_code(KC_9, CTRL | SHFT, RF | RR | RP | RB);        // cursive - wrap ()
  symbol_code(KC_EQL, SHFT, RF | RR);                       // +
  symbol_code(KC_7, SHFT, RF | RP | RL | RT);               // &
  symbol_code(KC_1, SHFT, RF | RP | RL);                    // !
  symbol_code(KC_9, SHFT, RF | RP );                        // (
  symbol_code_2(KC_DOT, KC_MINS, NOMODS, NOMODS, RF | RL);  // .-
  symbol_code(KC_DOT, NOMODS, RF);                          // .

  // Starts with -R
  symbol_code(KC_GRV, SHFT, RR | RP | RL | RT);             // ~
  symbol_code(KC_SLSH, SHFT, RR | RP | RL);                 // ?
  symbol_code(KC_6, SHFT, RR | RP | RG);                    // ^
  symbol_code(KC_SLSH, NOMODS, RR | RP);                    // /
  symbol_code(KC_4, SHFT, RR | RB | RG | RS);               // $
  symbol_code(KC_2, SHFT, RR | RB | RG);                    // @
  symbol_code(KC_0, SHFT, RR | RB);                         // )
  symbol_code(KC_COMM, NOMODS, RR);                         // ,

  // Starts with -P
  symbol_code(KC_LBRC, CTRL, RP | RB | RL | RG);            // cursive - wrap []
  symbol_code(KC_8, SHFT, RP | RB);                         // *
  symbol_code(KC_3, SHFT, RP | RL | RT);                    // #
  symbol_code(KC_LBRC, NOMODS, RP | RL);                    // [
  symbol_code(KC_BSLS, SHFT, RP | RG | RS);                 // |
  symbol_code(KC_BSLS, NOMODS, RP | RG);                    // backslash
  symbol_code(KC_SCLN, SHFT, RP);                           // :

  // Starts with -B
  symbol_code(KC_5, SHFT, RB | RG | RS);                    // %
  symbol_code(KC_RBRC, NOMODS, RB | RG);                    // ]
  symbol_code(KC_SCLN, NOMODS, RB);                         // ;

  // Starts with -L
  symbol_code(KC_LBRC, CTRL | SHFT, RL | RG | RT | RS);     // cursive - wrap {}
  symbol_code(KC_EQL, NOMODS, RL | RG);                     // =
  symbol_code_2(KC_MINS, KC_DOT, NOMODS, SHFT, RL | RT | RD);  // ->
  symbol_code(KC_LBRC, SHFT, RL | RT);                      // {
  symbol_code(KC_GRV, NOMODS, RL | RS);                     // `
  symbol_code(KC_MINS, NOMODS, RL);                         // -

  // Starts with -G
  symbol_code_3(KC_MINS, KC_DOT, KC_DOT, NOMODS, SHFT, SHFT, RG | RS | RZ);    // ->>
  symbol_code(KC_RBRC, SHFT, RG | RS);                      // }
  symbol_code(KC_MINS, SHFT, RG);                           // _

  // Starts with -T
  symbol_code(KC_QUOT, CTRL | SHFT, RT | RS);               // cursive - wrap ""
  symbol_code(KC_COMM, SHFT, RT | RD);                      // <
  symbol_code(KC_QUOT, SHFT, RT);                           // "

  // Starts with -S
  symbol_code(KC_DOT, SHFT, RS | RZ);                       // >
  symbol_code(KC_QUOT, NOMODS, RS);                         // '

  // Starts with -D
  symbol_code(KC_GRV, NOMODS, RD);                          // '

  // Starts with -Z
  symbol_code(KC_GRV, SHFT, RZ);                            // ~
}

bool process_symbols(uint32_t bitmask, keyrecord_t *record) {
    if (record->event.pressed) {
      return true;
    }
    else {
      if (chord_match(f_chord, symbol_bitmask))
      {
        symbol_chords();
      }
      if ((bitmask == symbol_bitmask)) {
        f_chord &= ~bitmask;
      }
    }
    return true;
}

  //////////////////////////////
 //         Numbers          //
//////////////////////////////

void number_code(uint32_t kc, uint32_t bitmask, bool twice) {
  // Doesn't clear the number bar until released
  // Must press the doubling key or reverse key again on each chord
  if (chord_match(f_chord, bitmask)) {
    SEND(kc);
    if (twice == true) {
      SEND(kc);
    }
    f_chord &= ~bitmask;
  }
}


void number_chords(void){
  bool twice = chord_match(f_chord, number_doubling_bitmask);      // Send each digit twice
  if (chord_match(f_chord, number_reverse_bitmask)) {               // Reverse number order
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
    f_chord &= ~number_reverse_bitmask;              // Reverse is only active for a single chord
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

  if (twice) { f_chord &= ~number_doubling_bitmask; } // Doubling is only active for single chord
}

bool process_numbers(uint32_t bitmask, keyrecord_t *record) {
    if (record->event.pressed) {
      return true;
    }
    else {
      if (chord_match(f_chord, number_bitmask))
      {
        number_chords();
      }
      if ((bitmask == number_bitmask) ||
          (bitmask == number_doubling_bitmask) ||
          (bitmask == number_reverse_bitmask)) {
        f_chord &= ~bitmask;
      }
    }
    return true;
}


  //////////////////////////////
 //      Fingerspelling      //
//////////////////////////////


void f_code(uint32_t kc, uint32_t bitmask) {
  // Used for all normal keypresses in fingerspelling mode
  // Clears keys from chord once used
    if (chord_match(f_chord, bitmask)) {
        SEND(kc);
        f_chord = f_chord & ~bitmask;
      }
}


void fingerspelling_chords(void){
  // This function checks for all the standard fingerspelling rules in Steno Order
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
    fingerspelling_chords();
    return true;
}


  //////////////////////////////
 //       Main Section       //
//////////////////////////////

bool process_chord(uint32_t bitmask, keyrecord_t *record) {
  if (record->event.pressed) {
      // Add pressed key to chords
      chord = chord | bitmask;      // This should reflect the current full chord
      f_chord = f_chord | bitmask;
      m_chord = m_chord | bitmask;
      i_chord = i_chord | bitmask;

      current_mods = compare_and_set_mods(current_mods, mod_state());   // Activate the current modifier state

      if(chord_match(i_chord,  instant_bitmask)){
        process_instant(bitmask, record);
      }
      extract_modifier_chords();
  }

  else {
      // If the symbols are active, fingerspelling chords should not do anything
      if(chord_match(i_chord,  instant_bitmask)) {
        process_instant(bitmask, record);
      }
      else if ((chord_match(f_chord, number_bitmask)) ||
               (chord_match(f_chord, symbol_bitmask))) {
        process_numbers(bitmask, record);
        process_symbols(bitmask, record);
      }
      else {
        process_fingerspelling(bitmask, record);
      }

      i_chord = i_chord & ~bitmask;
      m_chord = m_chord & ~bitmask;   // Clear released key from the modifier chords

      // Refresh the current modifiers after firing other chords
      // Expected behavior: When releasing modifier keys, fire all chords and change
      //                    modifier state. When releasing non-modifier keys, fire all
      //                    chords and leave modifier state the same.
      current_mods = compare_and_set_mods(current_mods, mod_state());
    }
    return true;
}

uint32_t convert_to_bitmask(uint16_t keycode) {
  switch(keycode) {
    case F_SU:  case STN_S1:  return LSU;
    case F_SD:  case STN_S2:  return LSD;
    case F_TL:  case STN_TL:  return LFT;
    case F_KL:  case STN_KL:  return LK;
    case F_PL:  case STN_PL:  return LP;
    case F_WL:  case STN_WL:  return LW;
    case F_HL:  case STN_HL:  return LH;
    case F_RL:  case STN_RL:  return LR;
    case F_A:   case STN_A:   return LA;
    case F_O:   case STN_O:   return LO;
    case F_ST1: case STN_ST1: return ST1;
    case F_ST2: case STN_ST2: return ST2;
    case F_ST3: case STN_ST3: return ST3;
    case F_ST4: case STN_ST4: return ST4;
    case F_E:   case STN_E:   return RE;
    case F_U:   case STN_U:   return RU;
    case F_FR:  case STN_FR:  return RF;
    case F_RR:  case STN_RR:  return RR;
    case F_PR:  case STN_PR:  return RP;
    case F_BR:  case STN_BR:  return RB;
    case F_LR:  case STN_LR:  return RL;
    case F_GR:  case STN_GR:  return RG;
    case F_TR:  case STN_TR:  return RT;
    case F_SR:  case STN_SR:  return RS;
    case F_DR:  case STN_DR:  return RD;
    case F_ZR:  case STN_ZR:  return RZ;
    case F_LNO:               return instant_bitmask;
    case F_RNO:               return number_bitmask;
  }
  return 0;
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // There might be a much nicer way to do this, but I haven't looked at changing it yet.
  uint8_t curr_layer = biton32(layer_state);

  switch (keycode) {
    case F_LNO : case F_RNO:
      if (record->event.pressed) {
        prev_layer = curr_layer;
        swap_layer(prev_layer, F_LAYER);
        }
      else {
        chord |= plover_chord;
        swap_layer(curr_layer, prev_layer);
      }
    return process_chord(convert_to_bitmask(keycode), record);
  }

  if (curr_layer == S_LAYER) {
    // Pass chords to Plover when on steno layer
    return true;
  }
  else {
    return process_chord(convert_to_bitmask(keycode), record);
  }  
}


bool process_steno_user(uint16_t keycode, keyrecord_t *record)
{
  // I want to use this to deal with the edge cases involved when the
  // layer is switched after pressing a plover key down
  // So far I can get it to stop, but can't get it to ever turn back on again

  if (record->event.pressed) {
    plover_chord |= convert_to_bitmask(keycode);
  }
  else {
    plover_chord &= ~convert_to_bitmask(keycode);

    // if (interrupt_steno) {
    //   SEND(KC_3);
    //   return false;
    //   }
    // else { SEND(KC_2);
    // return true;}
  }
  return true;
}

void matrix_scan_user(void) {

};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Main layer, everything goes through here
    [S_LAYER] = LAYOUT_georgi(
    TO(F_LAYER), STN_S1, STN_TL, STN_PL, STN_HL, STN_ST1,         STN_ST3,  STN_FR, STN_PR, STN_LR, STN_TR, STN_DR,
    KC_NO      , STN_S2, STN_KL, STN_WL, STN_RL, STN_ST2,         STN_ST4,  STN_RR, STN_BR, STN_GR, STN_SR, STN_ZR,
                                 STN_A,  STN_O,  F_LNO,       F_RNO, STN_E,  STN_U
    ),
    [F_LAYER] = LAYOUT_georgi(
    TO(S_LAYER), F_SU,   F_TL,   F_PL,   F_HL,   F_ST1,           F_ST3,    F_FR,   F_PR,   F_LR,   F_TR,   F_DR,
    KC_NO      , F_SD,   F_KL,   F_WL,   F_RL,   F_ST2,           F_ST4,    F_RR,   F_BR,   F_GR,   F_SR,   F_ZR,
                                  F_A,   F_O,    F_LNO,          F_RNO,    F_E,    F_U
    ),


};
