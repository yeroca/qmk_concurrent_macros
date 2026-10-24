# Concurrent Macros for QMK

This Concurrent Macros QMK community module provides a macro capability that's better suited to game playing than the core QMK does.

Features:
 * Multiple macros can run simultaneously.  This is limited only by the user-modifiable `MAX_CONCURRENT_MACROS` (default 10)
 * Macros can set to repeat continuously, either in a toggle on/off mode, or while the key is held down
 * The repeatable section of a macro can be preceded by an optional _preamble_ and followed by an optional _postamble_ that only run once per macro activation
 * An optional animation flashes the key that triggered the macro while the macro is running, returning to the normal backlight level when the macro is done
 * Macro definition is compatible with the current VIA launcher without any modifications

## Macro definition

Special rarely-used keycodes are used to tell concurrent macros to repeat and how.
 * `KC_F21` : start macro repeat section.
 * `KC_F22` : repeat the keys/delays between `KC_F21` and `KC_F22` while the macro key is held down (momentary mode)
 * `KC_F23` : stop all other running macros immediately, and clear the keyboard state
 * `KC_F24` : repeat the keys/delays between `KC_F21` and `KC_F24` until the macro key is pressed again (toggle mode)

The "grammar" of a repeatable macro looks like this:

__[__ *preamble keys/delays* `{KC_F21}` __]__ *repeated keys/delays* `{KC_F24}` __|__ `{KC_F22}`__[__ *postamble keys/delays* __]__

So for example, let's say you want the macro to start with "abc" then repeat the string "def" as long as you hold the key down, then when you release it, it sends "ghi".  To do this you create a macro like this:

`abc{KC_F21}def{KC_F22}ghi`

Notes:

 * If you don't need a preamble, you just omit that part and also the  `{KC_F21}`.  In this case, `{KC_F22}` and `{KC_F24}` will repeat from the beginning of the macro.
 * If you don't need a postamble, just omit that part.
 * If you don't want a repeat section, just don't use any of the special keycodes at all.
 * A maximum of one preamble, one repeat section, and one postamble is allowed.  The code will catch most incorrect usages of the special keycodes, and then enter an error state.  If you have the animation activated, an error is displayed as a rapidly blinking key.
 * When a macro is in the error state, you can use VIA to correct the macro content, and then you can retrigger the macro to try again.


## Concurrency is the devil's play toy

One thing to be mindful of is that if you activate modifier keys, such as Shift, Ctrl, Alt, AltGr, and GUI, those modifiers will affect other currently running macros as well as anything you may be typing at the moment (even on a different keyboard).  So it is a very good practice to completely avoid using modifier keys in macros, unless you are running only the one macro and not providing any other keyboard input while it runs, for example you are using a single macro in a repeat mode, and not touching any other keys.

Also keep in mind that if you manually use any modifier keys while a macro is running, it will affect keys being received by the computer.  For example if you have a macro that is continuously sending "a", and then while it's running, you hold down the shift key, the computer will start seeing repeated "A" keys.

## Mouse control

If you add the following line to your keyboard's `config.h` file:
```
#define CONCURRENT_MACROS_MOUSE_ENABLE
```
you can emit mouse movements, button clicks, and wheel movements via a special keycode `KC_F20`.

The tradeoff is that this option requires more RAM due to the need for additional macro state, and it requires a bit more flash memory (around 500 bytes) for the additional logic.  Increasing RAM usage may reduce the number of macros that can be running simultaneously.  Requiring more flash memory means there is less flash available for the contents of the macros. 

The `KC_F20` syntax is as follows:

`{KC_F20}`_button state_`,`_x movement_`,`_y movement_`,`_v movement_`,`_h movement_`,`_repeat count_`,`_period_`;`

Note: fields are comma-separated, embedded spaces and other characters are not allowed, the command must be terminated with a semicolon after the final field.

|field|meaning|allowed values|
|---|---|---|
|_button state_|state of the mouse buttons|0 <= _button state_ <= 255 (see details below)|
|_x movement_|move mouse pointer in the x (horizontal) direction|-128 <= _x movement_ <= 127|
|_y movement_|move mouse pointer in the y (vertical) direction|-128 <= _y movement_ <= 127|
|_v movement_|move mouse wheel in the vertical direction|-128 <= _v movement_ <= 127|
|_h movement_|move mouse wheel in the horizontal direction|-128 <= _h movement_ <= 127|
|_repeat count_|repeat specified movement _repeat count_ times|0 <= _repeat count_ <= 9999|
|_period_|delay for _period_ ms between each of the repeated movements|0 <= _period_ <= 9999|

If a field is left empty, the value of the field defaults to 0.

|Button|value to add|
|---|---|
|1|1|
|2|2|
|3|4|
|4|8|
|5|16|
|6|32|
|7|64|
|8|128|

To press buttons 3 and 5 simultaneously, use the value (button 3) 4 + (button 5) 16 = 20

Example: press button 3 for 500ms then release it:
```
{KC_F20}4,,,,,1,500;{KC_F20},,,,,1,1;
```
(remember the empty fields have a value of 0)

To make the pointer move in the x direction by an amount greater than 127 (or -128), you can use the _repeat count_ field to move the pointer repeatedly by a smaller amount.  For example if you want to move the pointer by 500, set the _x movement_ field to 100, and the _repeat count_ field to 5.  If the amount to move doesn't divide evenly, use one _repeat count_ command to cover most of the distance, then use a final movement command for a smaller amount to cover the remaining distance.  For example, to move 527, you could repeat a 100 movement 5 times, then do a final movement of 27.  Delaying 5ms between each mouse movement would look like this:
```
{KC_F20},100,,,,5,5;{KC_F20},27,,,,1,5;
```
(remember the empty fields have a value of 0)

## Configuration

### Set up the macro capability

The QMK source tree you use must have the Community Modules feature, because the Concurrent Macros module utilizes it.  At the time of this writing (2025/02/27), the community modules feature is now available in the `master` branch of QMK.  To get the latest source code, run these commands:

```
git checkout master
git pull origin master
```

To install the module, run these commands:

```
cd modules
git submodule add https://github.com/yeroca/qmk_concurrent_macros.git
```

Add to your `keynap.json` file the following:
```
"modules": ["qmk_concurrent_macros/concurrent_macros"]
```

### Set up the animation

#### Monochrome LED keyboards

If you have a keyboard with monochrome LEDs which are individually controllable, create a file in your keyboard directory called `led_matrix_kb.inc`, at the same level as the `keymaps` directory, containing the following line:

`#include "../concurrent_macros_animation/concurrent_macros_anim_mono.h"`

To your keyboard's `config.h` file add:

`#define LED_MATRIX_CUSTOM_KB`

To your keyboard's `keyboard.json` file, add the following line inside of the `"led_matrix"` / `"animations"` section:

`            "concurrent_macros": true,`

#### RGB LED keyboards

If you have a keyboard with RGB LEDs which are individually controllable, create a file in your keyboard directory called `rgb_matrix_kb.inc`, at the same level as the `keymaps` directory, containing the following line:

`#include "../concurrent_macros_animation/concurrent_macros_anim_rgb.h"`

To your keyboard's `config.h` file add:

`#define RGB_MATRIX_CUSTOM_KB`

To your keyboard's `keyboard.json` file, add the following line inside of the `"rgb_matrix"` / `"animations"` section:

`            "concurrent_macros": true,`

#### For both monochrome and RGB keyboards

Optionally, you might want to:

 * Make concurrent macros the default animation by adding/setting this define in your keyboard's `config.h` file:

```
#define LED_MATRIX_DEFAULT_MODE LED_MATRIX_CUSTOM_CONCURRENT_MACROS
```

 * Set other undesired/unused animations to false in your `keyboard.json` file
 * Modify your keyboard layout .json file that is used by VIA to add this lighting animation option and remove any ones that you set to false in `keyboard.json`.


### Special keycodes can be changed

If your game (or other application) uses F21 through F24 for some purpose, and you want your macro to emit those keycodes, you'll need to change concurrent macros' special keycodes by adding new definitions in your keyboard's `config.h` file.  The defaults are shown below:
```
#define KC_HARD_STOP_OTHER_MACROS KC_F23
#define KC_REPEAT_UNTIL_TOGGLE_OFF KC_F24
#define KC_REPEAT_UNTIL_MACRO_KEY_UP KC_F22
#define KC_START_REPEAT KC_F21
#define KC_REPORT_MOUSE KC_F20
```
