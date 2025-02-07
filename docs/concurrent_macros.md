# Concurrent Macros for QMK

This Concurrent Macros QMK community module provides a macro capability that's better suited to game playing than the core QMK does.

Features:
 * Multiple macros can run simultaneously.  This is limited only by the user-modifiable `MAX_CONCURRENT_MACROS` (default 10)
 * Macros can set to repeat continuously, either in a toggle on/off mode, or while the key is held down
 * The repeatable section of a macro can be preceded by an optional _preamble_ and followed by an optional _postamble_ that only run once per macro activation
 * An optional animation flashes the key that triggered the macro while the macro is running, returning to the normal backlight level when the macro is done
 * Macro definition is compatible with the current VIA launcher without any modifications

## Macro Definition

Special rarely-used keycodes are used to tell concurrent macros to repeat and how.
 * `KC_F21` : start macro repeat section.
 * `KC_F22` : repeat the keys/delays between `KC_F21` and `KC_F22` while the macro key is held down (momentary mode)
 * `KC_F23` : stop all other running macros immediately, and clear the keyboard state
 * `KC_F24` : repeat the keys/delays between `KC_F21` and `KC_F24` until the macro key is pressed again (toggle mode)

The "grammar" of a repeatable macro looks like this:

__[__ *preamble keys/delays* `{KC_F21}` __]__ *repeated keys/delays* `{KC_F24}` __|__ `{KC_F22}`__[__ *postamble keys/delays* __]__

So for example, let's say you want the macro to start with "abc" then repeat the string "def" as long as you hold the key down, then when you release it, it sends "ghi".  To do this you create a macro like this:

`abc{KC_F21}def{KC_F22}ghi`

Note that if you don't need a preamble, you just omit that part and also `{KC_F21}`.  If you don't need a postamble, just omit that part.

## Concurrency is the devil's play toy

One thing to be mindful of is that if you activate modifier keys, such as Shift, Ctrl, Alt, AltGr, and GUI, those modifiers will affect other currently running macros as well as anything you may be typing at the moment (even on a different keyboard).  So it is a very good practice to completely avoid using modifier keys in macros, unless you are not running the macro concurrently with any other keyboard usage (e.g. you are using a single macro in a repeat mode, and not touching any other keys).

