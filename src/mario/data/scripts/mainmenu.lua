
key_space = 44
key_left = 80
key_right = 79

function initMap()
  map.mario = map.objects[1]
  map.mario.input = false
  map.mario.inposition = false
  screen:zoom(200)
  screen:camera(400,0)
  map.currentsel = {}
  map.currentsel.id = 1
  map.currentsel.x = 190
  map.currentsel.selected = false
  map.newgameshell = map:spawn("GreenShell", 245, 150)
  map.quitshell = map:spawn("GreenShell", 500, 120)
  audio:playmusic("Theme", true)

  map:spawn("Title", 180, 150)
  
  map.selected = "NewGame"
  game.lives = 5
end

function updateMap()

  -- Print out the menu
  if not map.currentsel.selected then 
    game:print(mainfont, "New game", 222, 145, 0.25, -0.2)
    game:print(mainfont, "Quit", 490, 115, 0.25, -0.2)
  end

  -- New game
  if map.mario:x() > map.currentsel.x+5 and not map.currentsel.selected then
    map.mario:accel(600, -1, 0)
    map.mario.inposition = false
  elseif map.mario:x() < map.currentsel.x-5 and not map.currentsel.selected then
    map.mario:accel(600, 1, 0)
    map.mario.inposition = false
  else
    map.mario.inposition = true
  end

  if map.newgameshell:x() > 650 then map.selected = "NewGame" elseif map.quitshell:x() > 650 then map.selected = "Quit" end
  if map.selected == "NewGame" and map.mario:x() > 650 then
    game.currentlevel = "data/maps/level2.map"
    map:load(game.currentlevel)
  end

  if map.selected == "Quit" and map.mario:x() > 650 then
    game:stop()
  end

  if input:keydown(key_space) and not map.currentsel.selected and map.mario.inposition and math.abs(map.mario:velx()) < 20 then
    jump(map.mario, 400)
    map.currentsel.selected = true
    audio:play("NormalJump")
  end

  if input:keydown(key_right) then
    map.currentsel.x = 450
    map.currentsel.id = 2
  elseif input:keydown(key_left) then
    map.currentsel.x = 190
    map.currentsel.id = 1
  end
  
  -- Move towards the shell
  if map.currentsel.selected and not map.mario.inair then
    if map.currentsel.id == 1 then
      if not (math.abs(map.mario:x() - map.newgameshell:x()) < 10) then
        map.mario:accel(600, 1, 0)
      end
    elseif map.currentsel.id == 2 then
      if not (math.abs(map.mario:x() - map.quitshell:x()) < 10) then
        map.mario:accel(600, 1, 0)
      end
    end
  end
  

  -- Keep the camera inside the map bounds
  if screen:camerax() < (screen:zoom() * screen:aspect()) then
    screen:camera(screen:zoom() * screen:aspect(), screen:cameray())
  end
  if screen:cameray() < screen:zoom() then
    screen:camera(screen:camerax(), screen:zoom())
  end
end
