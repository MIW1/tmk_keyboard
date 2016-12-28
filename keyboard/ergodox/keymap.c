/*
Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "action_code.h"
#include "action_macro.h"
#include "action_layer.h"
#include "bootloader.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"
#include "ergodox.h"

// Swedish defines for easier reading of layout
#define KC_AA   KC_LBRACKET
#define KC_AE   KC_QUOTE
#define KC_OE   KC_SCOLON
#define KC_PLS  KC_MINUS           /* +, ? and \ */
#define KC_QUOT KC_BSLASH          /* ' and * */
#define KC_MINS KC_SLASH           /* - and _ */
#define KC_LTE  KC_NONUS_BSLASH    /* <, > and | */
#define KC_GRV  KC_EQUAL           /* ' (accent) and ` */
#define KC_TLDE KC_RBRACKET        /* ", ^ and ~ */

// Alias for functions so keymap get's easier to read
/* layers */
#define KC_L0   KC_FN20
#define KC_L1   KC_FN21
#define KC_L2   KC_FN22
#define KC_L3   KC_FN23
/* layer+key combinations */
#define KC_ENR0 KC_FN24
#define KC_ENR3 KC_FN25
/* switched numbers and most common symbol without shift mod */
#define KC_INV1 KC_FN1
#define KC_INV2 KC_FN2
#define KC_INV3 KC_FN3
#define KC_INV4 KC_FN4
#define KC_INV5 KC_FN5
#define KC_INV6 KC_FN6
#define KC_INV7 KC_FN7
#define KC_INV8 KC_FN8
#define KC_INV9 KC_FN9
#define KC_INV0 KC_FN0
/* unique special char keys */
#define KC_LBRK KC_FN11           /* { and [  Left brackets */
#define KC_RBRK KC_FN12           /* } and ]  Right brackets */
#define KC_BSHA KC_FN13           /* Backslash and @ */
#define KC_RTDE KC_FN14           /* Modified tilde. ~/ sequence, ^ and " */
#define KC_IMNS KC_FN15           /* _ and -  Inverted minus */
#define KC_SCLN KC_FN16           /* Only semicolon */
#define KC_CLN  KC_FN17           /* Colon and pipe */
#define KC_MNSA KC_FN18           /* -, ' (accent) and ` (grav accent) */

/* ErgoDox keymap definition macro */
#define KEYMAP(                                                 \
                                                                \
    /* left hand, spatial positions */                          \
    k00,k01,k02,k03,k04,k05,k06,                                \
    k10,k11,k12,k13,k14,k15,k16,                                \
    k20,k21,k22,k23,k24,k25,                                    \
    k30,k31,k32,k33,k34,k35,k36,                                \
    k40,k41,k42,k43,k44,                                        \
                            k55,k56,                            \
                                k54,                            \
                        k53,k52,k51,                            \
                                                                \
    /* right hand, spatial positions */                         \
        k07,k08,k09,k0A,k0B,k0C,k0D,                            \
        k17,k18,k19,k1A,k1B,k1C,k1D,                            \
            k28,k29,k2A,k2B,k2C,k2D,                            \
        k37,k38,k39,k3A,k3B,k3C,k3D,                            \
                k49,k4A,k4B,k4C,k4D,                            \
    k57,k58,                                                    \
    k59,                                                        \
    k5C,k5B,k5A )                                               \
                                                                \
   /* matrix positions */                                       \
   {                                                            \
    { KC_##k00,KC_##k10,KC_##k20,KC_##k30,KC_##k40,KC_NO   },   \
    { KC_##k01,KC_##k11,KC_##k21,KC_##k31,KC_##k41,KC_##k51},   \
    { KC_##k02,KC_##k12,KC_##k22,KC_##k32,KC_##k42,KC_##k52},   \
    { KC_##k03,KC_##k13,KC_##k23,KC_##k33,KC_##k43,KC_##k53},   \
    { KC_##k04,KC_##k14,KC_##k24,KC_##k34,KC_##k44,KC_##k54},   \
    { KC_##k05,KC_##k15,KC_##k25,KC_##k35,KC_NO,   KC_##k55},   \
    { KC_##k06,KC_##k16,KC_NO,   KC_##k36,KC_NO,   KC_##k56},   \
                                                                \
    { KC_##k07,KC_##k17,KC_NO,   KC_##k37,KC_NO,   KC_##k57},   \
    { KC_##k08,KC_##k18,KC_##k28,KC_##k38,KC_NO,   KC_##k58},   \
    { KC_##k09,KC_##k19,KC_##k29,KC_##k39,KC_##k49,KC_##k59},   \
    { KC_##k0A,KC_##k1A,KC_##k2A,KC_##k3A,KC_##k4A,KC_##k5A},   \
    { KC_##k0B,KC_##k1B,KC_##k2B,KC_##k3B,KC_##k4B,KC_##k5B},   \
    { KC_##k0C,KC_##k1C,KC_##k2C,KC_##k3C,KC_##k4C,KC_##k5C},   \
    { KC_##k0D,KC_##k1D,KC_##k2D,KC_##k3D,KC_##k4D,KC_NO   }    \
   }

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    KEYMAP(  // layer 0 : default
        // left hand
        L2,  INV1,INV2,INV3,INV4,INV5,BSHA,
        LTE, Q,   W,   E,   R,   T,   LBRK,
        TAB, A,   S,   D,   F,   G,
        ESC, Z,   X,   C,   V,   B,   HOME,
        L1,  LEFT,UP,  DOWN,RGHT,
                                      DEL, F5,
                                           LALT,
                                 LSFT,BSPC,RCTL,
        // right hand
             L3,  INV6,INV7,INV8,INV9,INV0,PLS,
             RBRK,Y,   U,   I,   O,   P,   AA,
                  H,   J,   K,   L,   OE,  AE,
             END, N,   M,   COMM,DOT, IMNS,CLN,
                      QUOT,INS, RTDE,MNSA,SCLN,
        RGUI,RALT,
        PGUP,
        PGDN,ENT, SPC
    ),
    KEYMAP(  // layer 1 : regular numbers, and some minor utility stuff
        // left hand
        TRNS,1,   2,   3,   4,   5,   TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        L0,  TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,TRNS,LCTL,
        // right hand
             TRNS,6,   7,   8,   9,   0,   TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                  TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                       TRNS,TRNS,TLDE,TRNS,TRNS,
        TRNS,TRNS,
        TRNS,
        TRNS,TRNS,TRNS
    ),
    KEYMAP(  // layer 2 : f-keys, numpad, media keys, misc
        // left hand
        L0,  F1,  F2,  F3,  F4,  F5,  F12,
        TRNS,VOLD,VOLU,MPLY,MPRV,TRNS,TRNS,
        TRNS,TRNS,MUTE,MSTP,MNXT,TRNS,
        TRNS,TRNS,PSCR,PAUS,SLCK,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,TRNS,TRNS,
        // right hand
             TRNS,F6,  F7,  F8,  F9,  F10, F11, 
             TRNS,PAST,P7,  P8,  P9,  NLCK,TRNS,
                  PPLS,P4,  P5,  P6,  TRNS,TRNS,
             TRNS,PMNS,P1,  P2,  P3,  PSLS,TRNS,
                       P0,  PDOT,NO,  TRNS,TRNS,
        TRNS,TRNS,
        TRNS,
        TRNS,TRNS,TRNS
    ),
    KEYMAP(  // layer 3: gaming layer (some games can't bind special char keys
        // left hand
        TAB, 1,   2,   3,   4,   5,   6,
        H,   TRNS,TRNS,TRNS,TRNS,TRNS,END,
        J,   TRNS,TRNS,TRNS,TRNS,TRNS,
        K,   TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        L,   TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,SPC ,TRNS,
        // right hand
             L0,  6,   7,   8,   9,   0,   ESC,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                  TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                       TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,
        TRNS,
        TRNS,ENR0,TRNS
    ),
    KEYMAP(  // layer 4: gaming typing layer, resets to "gaming layer" on enter
        // left hand
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,TRNS,TRNS,
        // right hand
             L0,  TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                  TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                       TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,
        TRNS,
        TRNS,ENR3,TRNS
    ),
};

/* id for user defined functions */
enum function_id {
    INV_0,
    INV_1,
    INV_2,
    INV_3,
    INV_4,
    INV_5,
    INV_6,
    INV_7,
    INV_8,
    INV_9,
    FN_LCBRACKET,
    FN_RCBRACKET,
    FN_TILDE,
    FN_UNDERSCR,
    FN_MINUS_ACCENT,
    FN_COLON_PIPE,
    FN_BASHSLAH_AT,
    SET_L1_WITH_LED,
    SET_L2_WITH_LED,
    SET_L3_WITH_LED,
    SET_L0_LED_OFF,
    ENR0,
    ENR3,
};

/*
 * Fn action definition
 */
static const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_FUNCTION(INV_0),
    [1] = ACTION_FUNCTION(INV_1),
    [2] = ACTION_FUNCTION(INV_2),
    [3] = ACTION_FUNCTION(INV_3),
    [4] = ACTION_FUNCTION(INV_4),
    [5] = ACTION_FUNCTION(INV_5),
    [6] = ACTION_FUNCTION(INV_6),
    [7] = ACTION_FUNCTION(INV_7),
    [8] = ACTION_FUNCTION(INV_8),
    [9] = ACTION_FUNCTION(INV_9),
    [11] = ACTION_FUNCTION(FN_LCBRACKET),
    [12] = ACTION_FUNCTION(FN_RCBRACKET),
    [13] = ACTION_FUNCTION(FN_BASHSLAH_AT),
    [14] = ACTION_FUNCTION(FN_TILDE),
    [15] = ACTION_FUNCTION(FN_UNDERSCR),
    [16] = ACTION_MODS_KEY(MOD_RSFT, KC_COMM),  // ;
    [17] = ACTION_FUNCTION(FN_COLON_PIPE),
    [18] = ACTION_FUNCTION(FN_MINUS_ACCENT),
    [20] = ACTION_FUNCTION(SET_L0_LED_OFF),
    [21] = ACTION_FUNCTION(SET_L1_WITH_LED),
    [22] = ACTION_FUNCTION(SET_L2_WITH_LED),
    [23] = ACTION_FUNCTION(SET_L3_WITH_LED),
    [24] = ACTION_FUNCTION(ENR0),
    [25] = ACTION_FUNCTION(ENR3),
};

/* Set layer 3 and light up led on teensy board.
 * Gaming layer
 */
void set_layer3_with_led(void) {
    ergodox_right_led_3_off();
    ergodox_right_led_2_off();
    ergodox_right_led_1_on();
    layer_clear();
    layer_on(3);
}

/* Set layer 2 and light up led on teensy board.
 * Function layer
 */
void set_layer2_with_led(void) {
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_on();
    layer_clear();
    layer_on(2);
}

/* Set layer 1 and light up led on teensy board.
 * Number layer
 */
void set_layer1_with_led(void) {
    ergodox_right_led_3_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_on();
    layer_clear();
    layer_on(1);
}

/* Set layer 0 and light up led on teensy board.
 */
void set_layer0_switch_led_off(void) {
    layer_clear();
    ergodox_right_led_3_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
}

/* add a shifted mod if no mod,
 * remove shited mod if shift mod only,
 * let all other mod combinations through without alteration.
 */
void inv_key(uint8_t key) {
    // invert shifting
    if (get_mods() == MOD_BIT(KC_LSHIFT)) {
        del_mods(MOD_BIT(KC_LSHIFT));
    } else if (get_mods() == 0) {  // don't use else, this lets other mod combos through
        add_mods(MOD_BIT(KC_LSHIFT));
    }
    register_code(key);
    // reset mods, otherwise quick keypresses can result in bugs (wrong mod state)
    if (get_mods() == MOD_BIT(KC_LSHIFT)) {
        del_mods(MOD_BIT(KC_LSHIFT));
    } else if (get_mods() == 0) {  // don't use else, this lets other mod combos through
        add_mods(MOD_BIT(KC_LSHIFT));
    }
}

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    keyevent_t event = record->event;
    tap_t tap = record->tap;

    switch (id) {
        case INV_0:
            if (event.pressed) {
                inv_key(KC_0);
            } else {
                unregister_code(KC_0);
            }
            break;
        case INV_1:
            if (event.pressed) {
                inv_key(KC_1);
            } else {
                unregister_code(KC_1);
            }
            break;
        case INV_2:
            if (event.pressed) {
                inv_key(KC_2);
            } else {
                unregister_code(KC_2);
            }
            break;
        case INV_3:
            if (event.pressed) {
                inv_key(KC_3);
            } else {
                unregister_code(KC_3);
            }
            break;
        case INV_4:
            if (event.pressed) {
                uint8_t mods = get_mods();
                if (mods == 0) {  // $
                    set_mods(MOD_BIT(KC_RALT));
                    register_code(KC_4);
                    clear_mods();
                } else if (mods == MOD_BIT(KC_LSHIFT)) {  // 4
                    clear_mods();
                    register_code(KC_4);
                    set_mods(MOD_BIT(KC_LSHIFT));
                } else if (mods == MOD_BIT(KC_RALT)) {  // ¤
                    set_mods(MOD_BIT(KC_LSHIFT));
                    register_code(KC_4);
                    set_mods(MOD_BIT(KC_RALT));
                }
            } else {
                unregister_code(KC_4);
            }
            break;
        case INV_5:
            if (event.pressed) {
                inv_key(KC_5);
            } else {
                unregister_code(KC_5);
            }
            break;
        case INV_6:
            if (event.pressed) {
                inv_key(KC_6);
            } else {
                unregister_code(KC_6);
            }
            break;
        case INV_7:
            if (event.pressed) {
                inv_key(KC_7);
            } else {
                unregister_code(KC_7);
            }
            break;
        case INV_8:
            if (event.pressed) {
                inv_key(KC_8);
            } else {
                unregister_code(KC_8);
            }
            break;
        case INV_9:
            if (event.pressed) {
                inv_key(KC_9);
            } else {
                unregister_code(KC_9);
            }
            break;
        case FN_LCBRACKET:
            if (event.pressed) {
                if (get_mods() == MOD_BIT(KC_LSHIFT)) {  // [ if shifted
                    set_mods(MOD_BIT(KC_RALT));
                    send_keyboard_report();
                    register_code(KC_8);
                    set_mods(MOD_BIT(KC_LSHIFT));
                } else {  // { otherwise
                    set_mods(MOD_BIT(KC_RALT));
                    send_keyboard_report();
                    register_code(KC_7);
                    clear_mods();
                }
            } else {
                unregister_code(KC_8);
                unregister_code(KC_7);
            }
            break;
        case FN_RCBRACKET:
            if (event.pressed) {
                if (get_mods() == MOD_BIT(KC_LSHIFT)) {  // ] if shifted
                    set_mods(MOD_BIT(KC_RALT));
                    register_code(KC_9);
                    set_mods(MOD_BIT(KC_LSHIFT));
                } else {  // } otherwise
                    set_mods(MOD_BIT(KC_RALT));
                    register_code(KC_0);
                    clear_mods();
                }
            } else {
                unregister_code(KC_9);
                unregister_code(KC_0);
            }
            break;
        case FN_TILDE:
            if (event.pressed) {
                uint8_t mods = get_mods();
                if (mods == 0) {  // ~/
                    set_mods(MOD_BIT(KC_RALT));
                    send_keyboard_report();
                    register_code(KC_TLDE);
                    unregister_code(KC_TLDE);
                    // don't want to also press space for tilde
                    register_code(KC_SPC);
                    unregister_code(KC_SPC);
                    clear_mods();
                    send_keyboard_report();
                    // (almost) always writing ~/ anyways so add the /
                    inv_key(KC_7);
                    unregister_code(KC_7);
                } else if (mods == MOD_BIT(KC_LSHIFT)) {  // ^
                    register_code(KC_TLDE);
                } else if (mods == MOD_BIT(KC_RALT)) {  // " 
                    clear_mods();
                    register_code(KC_TLDE);
                    set_mods(MOD_BIT(KC_RALT));
                }
            } else {
                uint8_t mods = get_mods();
                // Nothing to unregister if mods == 0, which means ~
                if (mods != 0) {
                    unregister_code(KC_TLDE);
                }
            }
            break;
        case FN_COLON_PIPE:
            if (event.pressed) {
                if (get_mods() == MOD_BIT(KC_LSHIFT)) {  // | if shifted
                    set_mods(MOD_BIT(KC_RALT));
                    register_code(KC_LTE);
                    set_mods(MOD_BIT(KC_LSHIFT));
                } else {  // : otherwise
                    set_mods(MOD_BIT(KC_LSHIFT));
                    register_code(KC_DOT);
                    clear_mods();
                }
            } else {
                unregister_code(KC_LTE);
                unregister_code(KC_DOT);
            }
            break;
        case FN_BASHSLAH_AT:
            if (event.pressed) {
                if (get_mods() == MOD_BIT(KC_LSHIFT)) {  // @ if shifted
                    set_mods(MOD_BIT(KC_RALT));
                    register_code(KC_2);
                    set_mods(MOD_BIT(KC_LSHIFT));
                } else {  // \ otherwise
                    set_mods(MOD_BIT(KC_RALT));
                    register_code(KC_PLS);
                    clear_mods();
                }
            } else {
                unregister_code(KC_2);
                unregister_code(KC_PLS);
            }
            break;
        case FN_MINUS_ACCENT:
            if (event.pressed) {
                uint8_t mods = get_mods();
                if (mods == 0) {  // -
                    register_code(KC_MINS);
                } else if (mods == MOD_BIT(KC_LSHIFT)) {  // ' (accent)
                    clear_mods();
                    register_code(KC_GRV);
                    set_mods(MOD_BIT(KC_LSHIFT));
                } else if (mods == MOD_BIT(KC_RALT)) {  // ` (grave accent)
                    set_mods(MOD_BIT(KC_LSHIFT));
                    register_code(KC_GRV);
                    set_mods(MOD_BIT(KC_RALT));
                }
            } else {
                unregister_code(KC_MINS);
                unregister_code(KC_GRV);
            }
            break;
        case FN_UNDERSCR:
            if (event.pressed) {
                inv_key(KC_MINS);
            } else {
                unregister_code(KC_MINS);
            }
            break;
        case SET_L0_LED_OFF:
            if (event.pressed) {
                set_layer0_switch_led_off();
            }
            break;
        case SET_L1_WITH_LED:
            if (event.pressed) {
                set_layer1_with_led();
            }
            break;
        case SET_L2_WITH_LED:
            if (event.pressed) {
                set_layer2_with_led();
            }
            break;
        case SET_L3_WITH_LED:
            if (event.pressed) {
                set_layer3_with_led();
            }
            break;
        case ENR0:
            if (event.pressed) {
                register_code(KC_ENTER);
            } else {
                unregister_code(KC_ENTER);
                layer_clear();
                layer_on(4);
            }
            break;
        case ENR3:
            if (event.pressed) {
                register_code(KC_ENTER);
            } else {
                unregister_code(KC_ENTER);
                layer_clear();
                layer_on(3);
            }
            break;
    }
}



#define KEYMAPS_SIZE    (sizeof(keymaps) / sizeof(keymaps[0]))
#define FN_ACTIONS_SIZE (sizeof(fn_actions) / sizeof(fn_actions[0]))

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, key_t key)
{
    if (layer < KEYMAPS_SIZE) {
        return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
    } else {
        // fall back to layer 0
        return pgm_read_byte(&keymaps[0][(key.row)][(key.col)]);
    }
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    action_t action;
    if (FN_INDEX(keycode) < FN_ACTIONS_SIZE) {
        action.code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]);
    } else {
        action.code = ACTION_NO;
    }
    return action;
}
