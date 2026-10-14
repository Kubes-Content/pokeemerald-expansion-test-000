[x] tick monster's behavior
    [x] calculate next direction if not blocked
    [x] temp cache current move dir ; 8-way direction until hit an edge (consider both paths for diagonals)
      counter each, reset on spawn
      [x] resets direction on spawn
      [x] checks both directions for diagonals
      [x] if all paths for chosen dir are blocked (and none are player) then reroll the direction
        [x] if not fully surrounded
    [x] tick
      [x] calculate next target direction
        [x] if all options are invalid (blocked by non-player, out of range)
    [x] avoids blocking warps