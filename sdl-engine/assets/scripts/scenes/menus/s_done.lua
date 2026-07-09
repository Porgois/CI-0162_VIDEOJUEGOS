scene = {
    -- Sprite table
    sprites = {
        [0] =
        {asset_id = "cursor", file_path = "./assets/ui/cursors/cursor.png"}
    },

    -- Font table
    fonts = {
        [0] = 
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
        { -- cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        { -- Struggle
            components = {
                text = {text = "The struggle continues...", font_id = "pixel_sleigh_32", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 120.0, y = 260.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- Thanks for playing
            components = {
                text = {text = "Thank you for playing!", font_id = "pixel_sleigh_32", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 140.0, y = 320.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- Back
            components = {
                clickable = {width = 100, height = 55, offset_x = -5, offset_y = -10},
                script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                text = {text = "Back", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                transform = {position = {x = 360.0, y = 500.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                tag = {tag = "t_back_button"}
            }
        }
    }
}

function scene.start()
    stop_all_sounds()
end
