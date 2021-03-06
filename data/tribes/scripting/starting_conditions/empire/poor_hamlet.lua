-- =======================================================================
--                Minimum Starting conditions for Empire
-- =======================================================================

include "scripting/infrastructure.lua"

set_textdomain("tribes")

return {
   -- TRANSLATORS: This is the name of a starting condition
   descname = _ "Poor Hamlet",
   -- TRANSLATORS: This is the tooltip for the "Poor Hamlet" (minimum) starting condition
   tooltip = _"Start the game with just a warehouse and the bare minimum for bootstrapping an economy. Warning: the AI can't successfully start from this",
   func =  function(player, shared_in_start)
      local sf = wl.Game().map.player_slots[player.number].starting_field
      if shared_in_start then
         sf = shared_in_start
      else
         player:allow_workers("all")
      end

      prefilled_buildings(player, { "empire_warehouse", sf.x, sf.y,
         wares = {
            granite = 1,
            log = 2,
            planks = 2,
            hammer = 2,
            felling_ax = 1,
            saw = 1,
            pick = 1,
            shovel = 1,
            -- Smelter:
            iron = 1,
            -- Farmer, fisher/hunter, geologist, innkeeper, miner, vinefarmer (x2):
            iron_ore = 7
         },
         soldiers = {
            [{0,0,0,0}] = 1
         }
      })

      player:reveal_fields(sf:region(10))
      player:conquer(sf, 9)
      player:send_message(_"Be careful", _"You have only one iron for each tool you will need to start your economy. Make sure no unneeded tool is created.")

      check_trees_rocks_poor_hamlet(player, sf, "empire_warehouse", {granite = 1}, {log = 1, planks = 1, granite = 1})
   end
}
