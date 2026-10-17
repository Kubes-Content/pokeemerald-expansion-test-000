[x] spawn over 16 objects
[ ] temp array of instance object states (default value is 'inactive')
	- OutdoorGroundObjectByInstanceIndex
	- needs to be set to 'inactive' when despawned
	- will save us if anything else is spawning objects
	- will allow us to query what's loaded
[ ] get objects to respawn from camera move
	[ ] 1
	[ ] 4
[ ] intercept pickup's interact script via instance id
	[ ] patch interact to pickup fruit
		[ ] add item to inventory
		[ ] add new item to inventory (create a new item definition)
		[ ] custom inventory cap (8 cherries = 1 item, 9 cherries = 2 items, 16 = 2 items)
			this way we can get multiple stacks per item with pokemon's bag
			each stack being separate would be a nice bonus
				should be pretty simple since the menu item probably just triggers something like "Use Once"
					then just make sure the list reloads properly when a stack changes/depletes
	[ ] spawn fruit from town save data
		[ ] pickup interaction persists
	[ ] pickup fruit
		[ ]	remove object
		[ ] add item through fruit type

[ ] NPCs should navigate independent of an object event to tick+move/collide while unloaded
	unloaded NPCs are included in collision and operate as loaded ones do
	and the object event rides on its back
		object loads in when they re-enter or are immediately off-screen

---

[ ] holding variable button (start button) opens bag to last picked up item
	[ ] player assignable keymaps

[ ] I need a bug that flies around
	[ ] I must capture it with my net - with timing involved that isn't annoying