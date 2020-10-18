function initMap()
  screen:zoom(150)
  --audio:playmusic("Overworld1", true)
  map.fpstimer = Timer(true)
  map.fps = 0
  map.maptimer = Timer(true)
  map.goaltimer = Timer()
end

function updateMap()
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

  if map.fpstimer:reached(0.5) then
    map.fps = 1/frametime
    map.fpstimer:reset()
  end
end
