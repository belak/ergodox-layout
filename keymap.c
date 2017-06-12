#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "eeconfig.h"

#define LAYER_ON(pos) ((layer_state) & (1<<(pos)))
#define _______ KC_TRNS

#define EECONFIG_BELAK_MAGIC (uint16_t)0xBE42

// NOTE: This is just a number that's a bit beyond the end of what's already
// defined. As there is no other define we can base this on, it may need to be
// changed in the future. The initial value here is used as a placeholder with a
// magic word, similar to the normal eeconfig.
#define EECONFIG_BELAK               (uint16_t *)32

// It's worth noting that I'm really lazy. The correct way to do this would be
// how the normal eeconfig handles it and use a bitfield. However, the eeprom
// has a ton of space which isn't being used so I don't really care and have a
// separate byte for every setting.
#define EECONFIG_BELAK_SWAP_GUI_CTRL (uint8_t *)34

static uint8_t swap_gui_ctrl = 0;

enum belak_keycodes {
  // Function codes
  BEL_F0 = SAFE_RANGE,
};

// TODO: Add LED support to the tap dance by using the advanced macro
#define LTOGGLE TD(TD_LAYER_TOGGLE)

#define BASE 0 // default layer
#define SYMB 1 // symbols
#define NUMP 2 // numpad
#define SWPH 3 // swap gui/ctrl on the hands

enum belak_td {
    TD_LAYER_TOGGLE = 0,
};

qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_LAYER_TOGGLE] = ACTION_TAP_DANCE_DOUBLE(TG(SYMB), TG(NUMP)),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Keymap 0: Basic layer
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |   Esc  |   1  |   2  |   3  |   4  |   5  |  L1  |           |  L2  |   6  |   7  |   8  |   9  |   0  |   =    |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |   Tab  |   Q  |   W  |   E  |   R  |   T  |  [   |           |   ]  |   Y  |   U  |   I  |   O  |   P  |   -    |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |    \   |   A  |   S  |   D  |   F  |   G  |------|           |------|   H  |   J  |   K  |   L  |   ;  |   '    |
 * |--------+------+------+------+------+------| LGui |           | RGui |------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |      |           |      |   N  |   M  |   ,  |   .  |   /  | RShift |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   | ~L1  | LCtrl| Left | Right| LAlt |                                       | RAlt | Up   | Down | RCtrl|  ~L2 |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,--------------.
 *                                        |  ~L2 | Ins  |       | Grv  | ~L1   |
 *                                ,-------|------|------|       |------+-------+-------.
 *                                | Back  |      | Home |       | PgUp |       |       |
 *                                | Space | Del  |------|       |------| Enter | Space |
 *                                |       |      | End  |       | PgDn |       |       |
 *                                `---------------------'       `----------------------'
 */
[BASE] = KEYMAP(  // layer 0 : default
        // left hand
        KC_ESC,         KC_1,         KC_2,   KC_3,   KC_4,   KC_5,   TG(SYMB),
        KC_TAB,         KC_Q,         KC_W,   KC_E,   KC_R,   KC_T,   KC_LBRC,
        CTL_T(KC_BSLS), KC_A,         KC_S,   KC_D,   KC_F,   KC_G,
        KC_LSFT,        KC_Z,         KC_X,   KC_C,   KC_V,   KC_B,   KC_LGUI,
        LTOGGLE,        KC_LCTRL,     KC_LEFT,KC_RGHT,KC_LALT,
                                                     MO(NUMP),KC_INS,
                                                              KC_HOME,
                                 CTL_T(KC_BSPC),GUI_T(KC_DEL),KC_END,
        // right hand
             TG(NUMP),    KC_6,   KC_7,   KC_8,   KC_9,   KC_0,             KC_EQL,
             KC_RBRC,     KC_Y,   KC_U,   KC_I,   KC_O,   KC_P,             KC_MINS,
                          KC_H,   KC_J,   KC_K,   KC_L,   KC_SCLN,          KC_QUOT,
             KC_RGUI,     KC_N,   KC_M,   KC_COMM,KC_DOT, KC_SLSH,          KC_RSFT,
                                  KC_RALT,KC_UP,  KC_DOWN,KC_RCTRL,         LTOGGLE,
             KC_GRV,      MO(SYMB),
             KC_PGUP,
             KC_PGDN,     GUI_T(KC_ENT), CTL_T(KC_SPC)
    ),
/* Keymap 1: Symbol Layer
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |  F1  |  F2  |  F3  |  F4  |  F5  |      |           |      |  F6  |  F7  |  F8  |  F9  |  F10 |   F11  |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |   !  |   @  |   {  |   }  |   |  |      |           |      |   Up |      |  Up  |      |      |   F12  |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |   #  |   $  |   (  |   )  |   `  |------|           |------| Down | Left | Down | Rght |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |   %  |   ^  |   [  |   ]  |   ~  |      |           |      |   &  |      |      |      |      |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      |      |      |                                       |      |      |      |      | TOGL |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      |      |       |      |      |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |      |      |      |       |      |      |      |
 *                                 |      |      |------|       |------|      |      |
 *                                 |      |      |      |       |      |      |      |
 *                                 `--------------------'       `--------------------'
 */
[SYMB] = KEYMAP(
       // left hand
       _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,
       _______, KC_EXLM, KC_AT,   KC_LCBR, KC_RCBR, KC_PIPE, _______,
       _______, KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN, KC_GRV,
       _______, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD, _______,
       _______, _______, _______, _______, _______,
                                                    _______, _______,
                                                             _______,
                                           _______, _______, _______,
       // right hand
       _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
       _______, KC_UP,   _______, KC_UP,   _______, _______, KC_F12,
                KC_DOWN, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
       _______, KC_AMPR, _______, _______, _______, _______, _______,
                         _______, _______, _______, _______, BEL_F0,
       _______, _______,
       _______,
       _______, _______, _______
),
/* Keymap 2: Numpad Layer
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |      |      |      |      |      |      |           |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |           |      |      |   7  |   8  |   9  |   *  |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |------|           |------|      |   4  |   5  |   6  |   +  |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |           |      |      |   1  |   2  |   3  |   \  |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      |      |      |                                       |   0  |    0 |   .  |   =  |      |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      |      |       |      |      |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |      |      |      |       |      |      |      |
 *                                 |      |      |------|       |------|      |      |
 *                                 |      |      |      |       |      |      |      |
 *                                 `--------------------'       `--------------------'
 */
[NUMP] = KEYMAP(
       // left hand
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______,
                                                    _______, _______,
                                                             _______,
                                           _______, _______, _______,
       // right hand
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, KC_7,    KC_8,    KC_9,    KC_ASTR, _______,
                _______, KC_4,    KC_5,    KC_6,    KC_PLUS, _______,
       _______, _______, KC_1,    KC_2,    KC_3,    KC_BSLS, _______,
                         KC_0,    KC_0,    KC_DOT,  KC_EQL,  _______,
       _______, _______,
       _______,
       _______, _______, _______
),
/* Keymap 31: Swap control and gui on the thumb
 */
[SWPH] = KEYMAP(  // layer 0 : default
        // left hand
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______,
                                                    _______, _______,
                                                             _______,
                              GUI_T(KC_BSPC), CTL_T(KC_DEL), _______,
        // right hand
       _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,
                _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,
                         _______, _______, _______, _______, _______,
       _______, _______,
       _______,
       _______, CTL_T(KC_ENT), GUI_T(KC_SPC)
    ),
};

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
    // If our magic word wasn't set properly, we need to zero out the settings.
    if (eeprom_read_word(EECONFIG_BELAK) != EECONFIG_BELAK_MAGIC) {
        eeprom_update_word(EECONFIG_BELAK, EECONFIG_BELAK_MAGIC);
        eeprom_update_byte(EECONFIG_BELAK_SWAP_GUI_CTRL, 0);
    }

    if (eeprom_read_byte(EECONFIG_BELAK_SWAP_GUI_CTRL)) {
        layer_on(SWPH);
        swap_gui_ctrl = 1;
    }
};

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();

    // Layer 1 and 2 are both overlay layers, so they could both be on. This
    // means we can't use the lazy check of checking for the first significant
    // bit.
    if (LAYER_ON(SYMB)) {
        ergodox_right_led_1_on();
    }
    if (LAYER_ON(NUMP)) {
        ergodox_right_led_2_on();
    }
};

bool process_record_user (uint16_t keycode, keyrecord_t *record) {
    if (keycode == BEL_F0 && record->event.pressed) {
        swap_gui_ctrl = !swap_gui_ctrl;
        eeprom_update_byte(EECONFIG_BELAK_SWAP_GUI_CTRL, swap_gui_ctrl);

        if (swap_gui_ctrl) {
            layer_on(SWPH);
        } else {
            layer_off(SWPH);
        }

        return false;
    }

    return true;
}
