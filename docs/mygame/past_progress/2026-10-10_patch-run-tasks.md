[x] tick task : one task should tick everything that's ours (no per mon)
what about a literal pre-warp, not just pre-dynamic-warp?
it'd be more reliable and as simple
[x] queue a cave task that ends when you leave
[x] patch run tasks
    It seems tasks are not ran during battle

or should we just patch what runs tasks to run our root task first without taking a task slot?
  wouldn't that run during battle?
    what if you knew what to check to skip during battle?
      gMain.inBattle
    what if I wanted to inject a root task for battle? wouldn't this simplify that?
    would run 4 all tasks? no?
1. SetUpFieldTasks inits root/overworld task
2. OnDungeonCellLoaded inits cell before task runs ... I think :D
3. SetUpFieldTasks reboots root/overworld task

[ ] change context pre-warp instead of on callback when map's loaded
  but if we have SetUpFieldTasks running what's in OnDungeonCellLoaded...
    ... than it will reinit the cell on battle end
  can we have SetUpFieldTasks just queue the root task?
  then OnDungeonCellLoaded initializes cell
    or we could patch wherever that's called ... another time
[ ] patch field_tasks.c:SetUpFieldTasks() for after battles
  WE SHOULD change context pre-warp
    we still have the option to override anything in OnCellLoaded if need be
  1. pre-warp            : set context
  2. SetUpFieldTasks     : switch on context to run what is atm in OnDungeonCellLoaded
  3. OnDungeonCellLoaded : no longer needs to do anything?
[ ] Battle kills task, so how do we resume without on cell loaded?
    Run+Win lead to CB2_ReturnToFieldLocal -> SetUpFieldTasks()
  is there a common intersection?
    to not need onCellLoaded?
  how do we resume task post-battle without making it resume when exiting to town
    post-battle patch + context check?