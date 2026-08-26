# Zephyr module build files for this config's custom ZMK behaviors.
# See zephyr/module.yml (module root is this repository).
target_sources(app PRIVATE src/behavior_esc_to_base.c)
