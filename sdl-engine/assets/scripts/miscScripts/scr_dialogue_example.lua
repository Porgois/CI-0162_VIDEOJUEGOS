function start()
    start_dialogue(this, {"Hello there!", "This is a typed dialogue line.", "Click again to dismiss it."}, "assets/soundEffects/misc/hits/hit_wall.wav", 64, 0.03)
end

function on_click()
    advance_dialogue(this)
end
