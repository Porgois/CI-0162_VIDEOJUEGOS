-- GLOBAL VARIABLES

var_name = "Isaac"
var_age = 24

print("[LUA] script01.lua")
print("[LUA] var_name: "..var_name)
print("[LUA] var_age: "..var_age)

config = {
    title = "2D Game Engine",
    fullscreen = true,
    resolution = {
        width = 800,
        height = 600
    }
}

function factorial(n)
    if n == 1 then
        return 1
    end
    return n * factorial(n -1)
end

var_pow = cPower(5, 2)
print("[LUA] power: "..var_pow)