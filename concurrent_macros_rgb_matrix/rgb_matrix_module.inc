#ifndef COMMUNITY_MODULE_CONCURRENT_MACROS_ENABLE
#    error "To use the concurrent_macros animation, you must add 'concurrent_macros' to your keyboard's module list in its keyboard.json file"
#endif
RGB_MATRIX_EFFECT(CONCURRENT_MACROS)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#    include "concurrent_macros.h"

/*
 * Negative RATE_ADJ makes the cycling faster, positive amounts make it
 * slower
 *
 * RATE_ADJ has the opposite sign as the user-visible define to make the
 * user setting more intuitive: larger values leads to a faster rate, and vice
 * versa.
 */
#ifndef COMMUNITY_MODULE_CONCURRENT_MACROS_RATE_ADJ
#    define RATE_ADJ 0
#else
#    define RATE_ADJ (-COMMUNITY_MODULE_CONCURRENT_MACROS_RATE_ADJ)
#endif

/*
 * Shift amounts must be positive, so we need to switch directions when the
 * shift amount is negative.
 */
#define SCALE_RATE(n, r) ((r + RATE_ADJ) < 0 ? n << -(r + RATE_ADJ) : n >> (r + RATE_ADJ))

#define FAST_RATE (SCALE_RATE(time, 0))
#define NORMAL_RATE (SCALE_RATE(time, 2))
#define SLOW_RATE (SCALE_RATE(time, 3))

static void iter_func(keypos_t* keypos, concurrent_macros_led_state_t led_state) {
    const uint8_t  led_index = g_led_config.matrix_co[keypos->row][keypos->col];
    const uint32_t time = scale16by8((uint16_t)(g_rgb_timer), rgb_matrix_config.speed);
    hsv_t next_hsv = { .s = 0xff, .v = 0xff }; // full saturation and brightness

    switch (led_state) {
        case MACRO_LED_STATE_RUNNING:
            next_hsv.h = NORMAL_RATE & 0xff;
            break;
        case MACRO_LED_STATE_STOPPING:
            next_hsv.h = SLOW_RATE & 0xff;
            break;
        case MACRO_LED_STATE_ERROR:
            next_hsv.h = FAST_RATE & 0xff;
            break;
        case MACRO_LED_STATE_IDLE:
        default:
            next_hsv = rgb_matrix_config.hsv;
            break;
    }
    const rgb_t next_rgb = hsv_to_rgb(next_hsv);
    rgb_matrix_set_color(led_index, next_rgb.r, next_rgb.g, next_rgb.b);
}

bool CONCURRENT_MACROS(effect_params_t* params) {
    const rgb_t rgb = hsv_to_rgb(rgb_matrix_config.hsv);
    rgb_matrix_set_color_all(rgb.r, rgb.g, rgb.b);
    concurrent_macros_macro_led_state_iterate(iter_func);

    return false; // false means we are done rendering
}
#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
