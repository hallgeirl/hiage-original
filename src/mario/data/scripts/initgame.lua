-- game.currentlevel = "data/maps/mainmenu.map"
game.currentlevel = "data/maps/level1.map"
map:load(game.currentlevel)
map:setflag("updateoffscreen", false)

mainfont = game:createfont("SmallFont")

game.lives = 5
game.score = 0
game.coins = 0
