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
        {font_id = "pixel_sleigh_30", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 30}
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
            { -- Death message
                components = {
                    text = {text = "Your plight will not be answered", font_id = "pixel_sleigh_30", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                    transform = {position = {x = 40.0, y = 260.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
                }
            },
            { -- Retry
                components = {
                    clickable = {width = 140, height = 55, offset_x = 0, offset_y = 0},
                    script = {path = "./assets/scripts/menuScripts/scr_retry_button.lua"},
                    text = {text = "Retry", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                    transform = {position = {x = 330.0, y = 340.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
                }
            },
            { -- Quit
                components = {
                    clickable = {width = 140, height = 55, offset_x = 0, offset_y = 0},
                    script = {path = "./assets/scripts/menuScripts/scr_menu_button.lua"},
                    text = {text = "Quit to menu", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255, outline_thickness = 3, is_ui = true},
                    transform = {position = {x = 270.0, y = 420.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
                    tag = {tag = "t_back_button"}
                }
            }
        }
}

function scene.start()
    stop_all_sounds()
end
