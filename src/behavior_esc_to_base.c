/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 *
 * Behavior: send Escape and force the default layer.
 *
 * On press: deactivates all layers (including any locked via a "sticky"
 * &tog key, since &to-style deactivation is locking) and returns to the
 * default layer, then emits Escape.
 * On release: releases Escape.
 *
 * Intended for a both-thumbs combo: cancel an operation / enter normal mode
 * in vim while also leaving any locked layer.
 */

#define DT_DRV_COMPAT zmk_behavior_esc_to_base

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <dt-bindings/zmk/keys.h>

#include <zmk/behavior.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Only the central role (or non-split keyboards) run the keymap engine; the
// peripheral half merely forwards scancodes over BLE. Guard the whole
// implementation so that on a peripheral the object references nothing
// (libapp.a is linked --whole-archive, so even an unused object must not
// pull in symbols like zmk_keymap_layer_to that don't exist there).
#if (!IS_ENABLED(CONFIG_ZMK_SPLIT)) || (IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL))

static int on_esc_to_base_pressed(struct zmk_behavior_binding *binding,
                                  struct zmk_behavior_binding_event event) {
    LOG_DBG("position %d: force default layer + ESC", event.position);
    zmk_keymap_layer_to(zmk_keymap_layer_default(), true);
    return raise_zmk_keycode_state_changed_from_encoded(ESCAPE, true, event.timestamp);
}

static int on_esc_to_base_released(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    LOG_DBG("position %d: release ESC", event.position);
    return raise_zmk_keycode_state_changed_from_encoded(ESCAPE, false, event.timestamp);
}

static const struct behavior_driver_api esc_to_base_driver_api = {
    .binding_pressed = on_esc_to_base_pressed,
    .binding_released = on_esc_to_base_released,
};

#define ESC_TO_BASE_INST(n)                                                                        \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &esc_to_base_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ESC_TO_BASE_INST)

#endif // (!IS_ENABLED(CONFIG_ZMK_SPLIT)) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
