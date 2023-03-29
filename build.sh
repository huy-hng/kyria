cd /home/huy/repositories/zmk/app

# west build -p -d build/left -b nice_nano_v2 -- -DSHIELD=kyria_rev3_left
# west build -p -d build/right -b nice_nano_v2 -- -DSHIELD=kyria_rev3_right

west build -p -d build/left  -b nice_nano_v2 -- -DSHIELD=kyria_rev3_left -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
west build -p -d build/right -b nice_nano_v2 -- -DSHIELD=kyria_rev3_right -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"

mv ./build/left/zephyr/zmk.uf2  /home/huy/repositories/zmk-config/build/left.uf2
mv ./build/right/zephyr/zmk.uf2 /home/huy/repositories/zmk-config/build/right.uf2
