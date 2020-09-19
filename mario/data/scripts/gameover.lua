function initMap()
  screen:zoom(200)
  map.maptimer = Timer(true)
end

function updateMap()
  screen:camera(500-500*math.cos(map.maptimer:get()/5), 100+50*math.sin(map.maptimer:get()))
end
