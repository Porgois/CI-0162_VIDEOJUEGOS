scene = {
    -- Sprite table
    sprites = (function() local t = dofile("./assets/scripts/misc/sprites.lua"); return t end)(),

    -- Font table
    fonts = {
        [0] =
        {font_id = "pixel_sleigh_18", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 18},
        {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24},
        {font_id = "pixel_sleigh_32", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 32}
    },

    -- Key & action table
    keys = {},

    -- Mouse buttons & actions table
    buttons = {
        [0] = 
        {name = "LMB", button = 1}
    },

    -- Map table
    maps = {
        width = 800,
        height = 600
    },

    -- Entity table
    entities = {
        [0] = 
        { -- Cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        { -- Controls
            components = {
                text = {text = "Controls", font_id = "pixel_sleigh_32", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 40.0, y = 80.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        dofile("./assets/scripts/entities/e_controls_keys.lua"),
        dofile("./assets/scripts/entities/e_controls_mouse.lua"),
        dofile("./assets/scripts/entities/e_controls_space.lua"),
        { -- Restart scene prompt
            components = {
                text = {text = "The \"0\" key restarts the current scene.", font_id = "pixel_sleigh_18", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 140.0, y = 500.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- Back
            components = {
                clickable = {width = 100, height = 55, offset_x = 0, offset_y = 0},
                script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                text = {text = "Back", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 40.0, y = 540.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                tag = {tag = "t_back_button"}
            }
        }
    }
}