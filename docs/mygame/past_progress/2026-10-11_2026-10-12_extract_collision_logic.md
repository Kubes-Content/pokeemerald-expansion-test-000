[x] Check for collision at spawn-time / runtime share logic
    [x] extract shared logic - none of these live in town_dungeon_persistent_data.*
      [x] check: is tile at coordinate passable
        - runtime kinda does this by itself
      [x] check: would object at coordinate block a warp
        [x] check: is warp at coordinate
        [x] check: would object at coordinate block northward warp
    [x] Check for collision at spawn-time
      [x] check: is object already spawned at coordinate - if need be this could also inherit runtime's object check
      [x] check: won't spawn if blocking warp
    [x] check for collision at runtime (not implemented yet)
      [x] check: is object at coordinate (not implemented for runtime yet)
      [x] movement needs to check for blocking warps (not implemented for runtime yet)