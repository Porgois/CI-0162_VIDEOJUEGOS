scene = {
    sprites = (function() local t = dofile("./assets/scripts/misc/sprites.lua"); return t end)(),

    fonts = {
        [0] =
        {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24},
        {font_id = "pixel_sleigh_32", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 32}
    },

    keys = {},

    buttons = {
        [0] =
        {name = "LMB", button = 1}
    },

    maps = {
        width = 800,
        height = 600
    },

    entities = {
        [0] =
         { -- cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        { -- dialogue box
            components = {
                clickable = {width = 720, height = 180, offset_x = 40, offset_y = 220},
                dialogue = {},
                script = {path = "./assets/scripts/miscScripts/scr_dialogue_scene_example.lua"},
                text = {text = "", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 2},
                transform = {position = {x = 40.0, y = 100.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- hint
            components = {
                text = {text = "Click the box to advance the dialogue", font_id = "pixel_sleigh_24", r = 180, g = 180, b = 180, a = 255, outline_thickness = 1},
                transform = {position = {x = 120.0, y = 430.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        }
    }
}

function scene.start()
    play_music("assets/soundEffects/misc/music/track_02.wav", -1, 10)
end
