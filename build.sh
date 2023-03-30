cd /home/huy/repositories/zmk/app

# west build -p -d build/left -b nice_nano_v2 -- -DSHIELD=kyria_rev3_left -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
# west build -p -d build/right -b nice_nano_v2 -- -DSHIELD=kyria_rev3_right -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"

west build -d build/left
west build -d build/right

cp ./build/left/zephyr/zmk.uf2  /run/media/huy/NICENANO/zmk.uf2
cp ./build/right/zephyr/zmk.uf2  /run/media/huy/NICENANO1/zmk.uf2

# cp ./build/left/zephyr/zmk.uf2  /home/huy/repositories/zmk-config/build/left.uf2
# cp ./build/right/zephyr/zmk.uf2 /home/huy/repositories/zmk-config/build/right.uf2
