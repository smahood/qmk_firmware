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

// Chord state
uint32_t cChord 		= 0;		// Current Chord
int		 chordIndex 	= 0;		// Keys in previousachord
int32_t  chordState[32];			// Full Chord history
#define  QWERBUF		24			// Size of chords to buffer for output

bool	 repeatFlag 	= false;	// Should we repeat?
uint32_t pChord 		= 0;		// Previous Chord
int		 pChordIndex 	= 0;		// Keys in previousachord
uint32_t pChordState[32];			// Previous chord state
uint32_t stickyBits = 0;			// Or'd with every incoming press

// Mode state
enum MODE { STENO = 0, QWERTY, COMMAND };
enum MODE pMode;
bool QWERSTENO = false;
#ifdef ONLYQWERTY
enum MODE cMode = QWERTY;
#else
enum MODE cMode = STENO;
#endif

// Command State
#define MAX_CMD_BUF   20
uint8_t	 CMDLEN 	= 0;
uint8_t	 CMDBUF[MAX_CMD_BUF];

// Key Repeat state
bool     inChord  		= false;
bool	 repEngaged 	= false;
uint16_t repTimer 		= 0;
#define  REP_INIT_DELAY 750
#define  REP_DELAY 		25


bool fingerspelling_pressed(uint32_t bitmask) {
    f_chord = f_chord | bitmask;
    return false;
}

void f_code(uint32_t kc, uint32_t bitmask) {
    if ((f_chord & bitmask) == bitmask) {
        register_code(kc);
        unregister_code(kc);
        f_chord = f_chord & ~bitmask;
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
  f_code(KC_BSPC, ST1);
  f_code(KC_BSPC, ST2);
  f_code(KC_BSPC, ST3);
  f_code(KC_BSPC, ST4);
}

bool process_fingerspelling(uint32_t bitmask, bool pressed) {
    if (pressed) {
        f_chord = f_chord | bitmask;
    } else if ((f_chord & bitmask) > 0) {
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
            // case F_ST1: return process_fingerspelling(LK,  record->event.pressed);
            // case F_ST2: return process_fingerspelling(LK,  record->event.pressed);
            // case F_ST3: return process_fingerspelling(LK,  record->event.pressed);
            // case F_ST4: return process_fingerspelling(LK,  record->event.pressed);
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


bool halt_chord_processing(void) {
  // This should be called by by all combinations that should not be treated as chords
	cChord = 0;
	inChord = false;
	chordIndex = 0;
	clear_keyboard();
	repEngaged  = false;
	for (int i = 0; i < 32; i++){
		chordState[i] = 0xFFFF;
  }
  return false;
}

bool continue_chord_processing(void) {
 	// Hey that's a steno chord!
	inChord = false;
	chordIndex = 0;
	cChord = 0;
	return true;
}


bool toggle_command_mode(steno_mode_t mode, uint8_t chord[6]) {
  #ifndef NO_DEBUG
		uprintf("COMMAND Toggle\n");
  #endif

  if (cMode != COMMAND) {   // Entering Command Mode
    CMDLEN = 0;
    pMode = cMode;
    cMode = COMMAND;
  } else {                  // Exiting Command Mode
    cMode = pMode;

    // Press all and release all
    for (int i = 0; i < CMDLEN; i++) {
      register_code(CMDBUF[i]);
    }
    clear_keyboard();
  }
  return halt_chord_processing();
}




// All processing done at chordUp goes through here
bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {

	// Toggle Serial/QWERTY steno
	if (cChord == (PWR | FN | ST1 | ST2)) {
#ifndef NO_DEBUG
		uprintf("Fallback Toggle\n");
#endif
		QWERSTENO = !QWERSTENO;

		return halt_chord_processing();
	}

	// handle command mode
	if (cChord == (PWR | FN | RD | RZ)) {
    return toggle_command_mode(mode, chord);
	}

	// Handle Gaming Toggle,
	if (cChord == (PWR | FN | ST4 | ST3) && keymapsCount > 1) {
#ifndef NO_DEBUG
		uprintf("Switching to QMK\n");
#endif
		layer_on(1);
		return halt_chord_processing();
	}

	// Lone FN press, toggle QWERTY
#ifndef ONLYQWERTY
	if (cChord == FN) {
		(cMode == STENO) ? (cMode = QWERTY) : (cMode = STENO);
		return halt_chord_processing();
	}
#endif

	// Check for Plover momentary
	if (cMode == QWERTY && (cChord & FN)) {
		cChord ^= FN;
		return continue_chord_processing();
	}

	// Do QWERTY and Momentary QWERTY
	if (cMode == QWERTY || (cMode == COMMAND) || (cChord & (FN | PWR))) {
		processChord(false);
		return halt_chord_processing();
	}

	// Fallback NKRO Steno
	if (cMode == STENO && QWERSTENO) {
		processChord(true);
		return halt_chord_processing();
	}

  return continue_chord_processing();
}

// Update Chord State
bool process_steno_user(uint16_t keycode, keyrecord_t *record) {
	// Everything happens in here when steno keys come in.
	// Bail on keyup
	if (!record->event.pressed) { return true; }
    else {
        repTimer = timer_read();
        inChord  = true;

        // Switch on the press adding to chord
        switch (keycode) {
            // Mods and stuff
            case STN_ST1:			(cChord |= (ST1)); break;
            case STN_ST2:			(cChord |= (ST2)); break;
            case STN_ST3:			(cChord |= (ST3)); break;
            case STN_ST4:			(cChord |= (ST4)); break;
            case STN_FN:			(cChord |= (FN)) ; break;
            case STN_PWR:			(cChord |= (PWR)); break;
            case STN_N1...STN_N6:	(cChord |= (LNO)); break;
            case STN_N7...STN_NC:	(cChord |= (RNO)); break;

            // All the letter keys
            case STN_S1:			(cChord |= (LSU)) ;  break;
            case STN_S2:			(cChord |= (LSD)) ;  break;
            case STN_TL:			(cChord |= (LFT)) ; break;
            case STN_KL:			(cChord |= (LK)); break;
            case STN_PL:			(cChord |= (LP)); break;
            case STN_WL:			(cChord |= (LW)); break;
            case STN_HL:			(cChord |= (LH)); break;
            case STN_RL:			(cChord |= (LR)); break;
            case STN_A:				(cChord |= (LA)); break;
            case STN_O:				(cChord |= (LO)); break;
            case STN_E:				(cChord |= (RE)); break;
            case STN_U:				(cChord |= (RU)); break;
            case STN_FR:			(cChord |= (RF)); break;
            case STN_RR:			(cChord |= (RR)); break;
            case STN_PR:			(cChord |= (RP)); break;
            case STN_BR:			(cChord |= (RB)); break;
            case STN_LR:			(cChord |= (RL)); break;
            case STN_GR:			(cChord |= (RG)); break;
            case STN_TR:			(cChord |= (RT)); break;
            case STN_SR:			(cChord |= (RS)); break;
            case STN_DR:			(cChord |= (RD)); break;
            case STN_ZR:			(cChord |= (RZ)); break;
        }

	// Store previous state for fastQWER
    chordState[chordIndex] = cChord;
    chordIndex++;

    return true;
    }
}

void matrix_scan_user(void) {
	// We abuse this for early sending of key
	// Key repeat only on QWER/SYMB layers
	if (cMode != QWERTY || !inChord) return;

	// Check timers
#ifndef NO_REPEAT
	if (repEngaged && timer_elapsed(repTimer) > REP_DELAY) {
		// Process Key for report
		processChord(false);

		// Send report to host
		send_keyboard_report();
		clear_keyboard();
		repTimer = timer_read();
	}

	if (!repEngaged && timer_elapsed(repTimer) > REP_INIT_DELAY) {
		repEngaged = true;
	}
#endif
};

// For Plover NKRO
uint32_t processFakeSteno(bool lookup) {
	P( LSU,				SEND(KC_Q););
	P( LSD,				SEND(KC_A););
	P( LFT,				SEND(KC_W););
	P( LP,				SEND(KC_E););
	P( LH,				SEND(KC_R););
	P( LK,				SEND(KC_S););
	P( LW,				SEND(KC_D););
	P( LR,				SEND(KC_F););
	P( ST1,				SEND(KC_T););
	P( ST2,				SEND(KC_G););
	P( LA,				SEND(KC_C););
	P( LO,				SEND(KC_V););
	P( RE,				SEND(KC_N););
	P( RU,				SEND(KC_M););
	P( ST3,				SEND(KC_Y););
	P( ST4,				SEND(KC_H););
	P( RF,				SEND(KC_U););
	P( RP,				SEND(KC_I););
	P( RL,				SEND(KC_O););
	P( RT,				SEND(KC_P););
	P( RD,				SEND(KC_LBRC););
	P( RR,				SEND(KC_J););
	P( RB,				SEND(KC_K););
	P( RG,				SEND(KC_L););
	P( RS,				SEND(KC_SCLN););
	P( RZ,				SEND(KC_COMM););
	P( LNO,				SEND(KC_1););
	P( RNO,				SEND(KC_1););

	return 0;
}

// Traverse the chord history to a given point
// Returns the mask to use
void processChord(bool useFakeSteno) {
	// Save the clean chord state
	uint32_t savedChord = cChord;

	// Apply Stick Bits if needed
	if (stickyBits != 0) {
		cChord |= stickyBits;
		for (int i = 0; i <= chordIndex; i++)
			chordState[i] |= stickyBits;
	}

	// Strip FN
	if (cChord & FN) cChord ^= FN;

	// First we test if a whole chord was passsed
	// If so we just run it handling repeat logic
	if (useFakeSteno && processFakeSteno(true) == cChord) {
		processFakeSteno(false);
		return;
	} else if (processQwerty(true) == cChord) {
		processQwerty(false);
		// Repeat logic
		if (repeatFlag) {
			restoreState();
			repeatFlag = false;
			processChord(false);
		} else {
			saveState(cChord);
		}
		return;
	}

	// Iterate through chord picking out the individual
	// and longest chords
	uint32_t bufChords[QWERBUF];
	int 	 bufLen		= 0;
	uint32_t mask		= 0;

	// We iterate over it multiple times to catch the longest
	// chord. Then that gets addded to the mask and re run.
	while (savedChord != mask) {
		uint32_t test  	 		= 0;
		uint32_t longestChord	= 0;

		for (int i = 0; i <= chordIndex; i++) {
			cChord = chordState[i] & ~mask;
			if (cChord == 0)
				continue;

			// Assume mid parse Sym is new chord
			if (i != 0 && test != 0 && (cChord ^ test) == PWR) {
				longestChord = test;
				break;
			}

			// Lock SYM layer in once detected
			if (mask & PWR)
				cChord |= PWR;


			// Testing for keycodes
			if (useFakeSteno) {
				test = processFakeSteno(true);
			} else {
				test = processQwerty(true);
			}

			if (test != 0) {
				longestChord = test;
			}
		}

		mask |= longestChord;
		bufChords[bufLen] = longestChord;
		bufLen++;

		// That's a loop of sorts, halt processing
		if (bufLen >= QWERBUF) {
			return;
		}
	}

	// Now that the buffer is populated, we run it
	for (int i = 0; i < bufLen ; i++) {
		cChord = bufChords[i];
		if (useFakeSteno) {
			processFakeSteno(false);
		} else {
			processQwerty(false);
		}
	}

	// Save state in case of repeat
	if (!repeatFlag) {
		saveState(savedChord);
	}

	// Restore cChord for held repeat
	cChord = savedChord;

	return;
}
void saveState(uint32_t cleanChord) {
	pChord = cleanChord;
	pChordIndex = chordIndex;
	for (int i = 0; i < 32; i++)
		pChordState[i] = chordState[i];
}
void restoreState() {
	cChord = pChord;
	chordIndex = pChordIndex;
	for (int i = 0; i < 32; i++)
		chordState[i] = pChordState[i];
}

// Macros for calling from keymap.c
void SEND(uint8_t kc) {
	// Send Keycode, Does not work for Quantum Codes
	if (cMode == COMMAND && CMDLEN < MAX_CMD_BUF) {
#ifndef NO_DEBUG
		uprintf("CMD LEN: %d BUF: %d\n", CMDLEN, MAX_CMD_BUF);
#endif
		CMDBUF[CMDLEN] = kc;
		CMDLEN++;
	}

	if (cMode != COMMAND) register_code(kc);
	return;
}
void REPEAT(void) {
	if (cMode != QWERTY)
		return;

	repeatFlag = true;
	return;
}
void SET_STICKY(uint32_t stick) {
	stickyBits = stick;
	return;
}
void SWITCH_LAYER(int layer) {
	if (keymapsCount >= layer)
		layer_on(layer);
}




// Proper Layers
#define FUNCT   (LSD | LK | LP | LH)
#define MEDIA   (LSD | LK | LW | LR)
#define MOVE    (ST1 | ST2)

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


uint32_t processFingerspellingChord(uint32_t chord);


void SEND_SHIFTED(uint8_t kc) {
    bool curr_shifted = (keyboard_report->mods & MOD_BIT(KC_LSHIFT));
    if (curr_shifted) {
        register_code(kc);
        unregister_code(kc);
        }
    else {
        register_code(KC_LSFT);
        register_code(kc);
        unregister_code(kc);
        unregister_code(KC_LSFT);
        }
}

void SEND_CTRLED(uint8_t kc) {
    bool curr_ctrled = (keyboard_report->mods & MOD_BIT(KC_LCTRL));
    if (curr_ctrled) {
        register_code(kc);
        unregister_code(kc);
        }
    else {
        register_code(KC_LCTRL);
        register_code(kc);
        unregister_code(kc);
        unregister_code(KC_LCTRL);
        }
}

void SEND_CTRL_SHIFTED(uint8_t kc) {
    bool curr_ctrled = (keyboard_report->mods & MOD_BIT(KC_LCTRL));
    bool curr_shifted = (keyboard_report->mods & MOD_BIT(KC_LSHIFT));

    if (!curr_ctrled) {
        register_code(KC_LCTRL);
    }

    if (!curr_shifted) {
        register_code(KC_LSFT);
    }

    register_code(kc);
    unregister_code(kc);

    if (!curr_ctrled) {
        unregister_code(KC_LCTRL);
    }

    if (!curr_shifted) {
        unregister_code(KC_LSFT);
    }
}

// void lsu(uint32_t chord){
//     register_code(KC_S);
//     unregister_code(KC_S);
//     processFingerspellingChord(chord &= ~LSU);
// }

// void lsd(uint32_t chord){
//     register_code(KC_S);
//     unregister_code(KC_S);
//     processFingerspellingChord(chord &= ~LSD);
// }

// void lft(uint32_t chord){
//     register_code(KC_T);
//     unregister_code(KC_T);
//     processFingerspellingChord(chord &= ~LFT);
// }


// uint32_t processFingerspellingChord(uint32_t chord) {
//     if (chord & LSU) {lsu(chord);}
//     else if (chord & LSD) {lsd(chord);}
//     else if (chord & LFT) {lft(chord);}

//     return 0;}



// Note: You can only use basic keycodes here!
// P() is just a wrapper to make your life easier.
//
// http://docs.gboards.ca
uint32_t processQwerty(bool lookup) {
    // Specials
    P( RT  | RS  | RD  | RZ | LNO,        SEND_STRING(VERSION); SEND_STRING(__DATE__));
    // processFingerspellingChord(cChord);
    //P(LSU, processFingerspellingChord(cChord));
    //P(LSD, processFingerspellingChord(cChord));
    //P(LFT, processFingerspellingChord(cChord));

    P(LSU | LFT | LK | LP | LW,             SEND(KC_Z));
    P(LSD | LFT | LK | LP | LW,             SEND(KC_Z));

    P(LSU | LK | LW | LR,                   SEND(KC_J));
    P(LSD | LK | LW | LR,                   SEND(KC_J));

    P(LSU | LR,                             SEND(KC_V));
    P(LSD | LR,                             SEND(KC_V));
    P(LSU | RP,                             SEND(KC_SPC));
    P(LSD | RP,                             SEND(KC_SPC));

    P(LSU,                                  SEND(KC_S));
    P(LSD,                                  SEND(KC_S));

    P(LFT | LK | LP | LW,                   SEND(KC_G));
    P(LFT | LP | LH,                        SEND(KC_N));
    P(LFT | LK,                             SEND(KC_D));
    P(LFT | LP,                             SEND(KC_F));
    P(LFT,                                  SEND(KC_T));


    P(LK | LW | LR,                         SEND(KC_Y));
    P(LK | LP,                              SEND(KC_X));
    P(LK | LW ,                             SEND(KC_Q));
    P(LK | LR ,                             SEND(KC_C));



    P(LP | LW ,                             SEND(KC_B));
    P(LP | LH ,                             SEND(KC_M));
    P(LP,                                   SEND(KC_P));

    P(LW,                                   SEND(KC_W));

    P(LH | LR ,                             SEND(KC_L));
    P(LH,                                   SEND(KC_H));

    P(LR,                                   SEND(KC_R));

    P(LA,                                   SEND(KC_A));

    P(LO,                                   SEND(KC_O));

    P(RE | RU,                              SEND(KC_I));
    P(RE,                                   SEND(KC_E));
    P(RU,                                   SEND(KC_U));






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



    return 0;
}

// "Layers"
// Steno layer should be first in your map.
// When PWR | FN | ST3 | ST4 is pressed, the layer is increased to the next map. You must return to STENO_LAYER at the end.
// If you need more space for chords, remove the two gaming layers.
// Note: If using NO_ACTION_TAPPING, LT will not work!

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Main layer, everything goes through here
    [STENO_LAYER] = LAYOUT_georgi(
    TO(FINGERSPELLING_LAYER),  STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,       STN_ST3, STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
    STN_PWR, STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2,       STN_ST4, STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
                               STN_A,   STN_O,  STN_N1,       MO(FINGERSPELLING_LAYER),  STN_E,   STN_U
    ),
    [FINGERSPELLING_LAYER] = LAYOUT_georgi(
    TO(STENO_LAYER),  F_SU,  F_TL,  F_PL,  F_HL,  F_ST1,       F_ST3, F_FR,  F_PR,  F_LR,  F_TR,  F_DR,
    F_PWR, F_SD,  F_KL,  F_WL,  F_RL,  F_ST2,       F_ST4, F_RR,  F_BR,  F_GR,  F_SR,  F_ZR,
                               F_A,   F_O,  F_NL,       MO(FINGERSPELLING_LAYER),  F_E,   F_U
    )
};
// Don't fuck with this, thanks.
size_t keymapsCount  = sizeof(keymaps)/sizeof(keymaps[0]);
