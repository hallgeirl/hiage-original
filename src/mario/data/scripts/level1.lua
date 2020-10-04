function initMap()
  screen:zoom(150)
  --audio:playmusic("Overworld1", true)
  map.fpstimer = Timer(true)
  map.fps = 0
  map.maptimer = Timer(true)
  map.goaltimer = Timer()
end

function updateMap()
  screen:camera(map.player:x(), map.player:y())
  
  if map.player.goal then
    -- When entering the goal
    map.player.goal = false
    map.goaltimer:start()
    map.player.input = false
  end
  
  if not map.goaltimer:reached(5) and map.goaltimer:reached(0.5) then
    map.player:accel(600, 1, 0)
    map.player.maxspeed = 50
  elseif map.goaltimer:reached(7) then
    map:load("data/maps/level2.json")
  end
  
  if screen:camerax() < (screen:zoom() * screen:aspect()) then
    screen:camera(screen:zoom() * screen:aspect(), screen:cameray())
  end

  if screen:cameray() < screen:zoom() then
    screen:camera(screen:camerax(), screen:zoom())
  end

  if map.fpstimer:reached(0.5) then
    map.fps = 1/frametime
    map.fpstimer:reset()
  end
  
  printLives(-200,130)
  printScore(-30,130)
  printCoins(130,130)
  game:print(mainfont, "FPS: " .. map.fps , screen:camerax() - 200, screen:cameray() + 115, 0.2, -0.2)
end
