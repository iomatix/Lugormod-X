# Extern Variables List
**They are usable inside server.cfg file.**

List of all Extern Variables added by iomatix:
- **lmd_jedi_pickup_weapons** 0 is default. 
  
  1 allows Force User to pick up weapons and ammo from the map. 
- **lmd_jedi_add_hp_level** 1 is default. 
  
  Allows Force User to extend HP capacity per level, 0 is blocking that feature.  
- **lmd_profession_fee** 1350 is default. 
  
  It's Credits cost of the profession change. If credits value is smaller than this value it will take all CR belonging to the player.
- **lmd_exp_for_level** 450 is default. 
  
  This value is used to calculate how many experience points player will need to gain the next level.
- **lmd_skillpoint_cost** 150 is default. 
  
  It's Credits cost of spending one skill point for upgrading skills. 
- **lmd_bounty_streaks_by** 5 is default. 
  
  That value is used in killing streak counter. After that value killing streak reward for killing the player is increasing.
- **lmd_skillpoints_perlevel** 4 is default. 

  That value determines how many skill points player will receive after each level-up. After reaching the Mastery Level the value slightly increasing. 
- **lmd_mastery_level_skills_level**
  
  That value determines max level of the skills before reaching Mastery Level. 3 is default. 
- **lmd_damage_level_scale** 
When is set to 1 the damage output will be scaled with level difference between attacker and target. 0 to turn off, 1 is default.

- **lmd_is_lethality_add_damage** 
When is set to 1 the damage output based on lethality skill will increase, with 0 the damage will be converted instead. default 0, 1 to turn it on.

- **lmd_is_thousandcuts_lethality** 
When is set to 1 the additional damage output is converted to the lethality, subtracting it from HP directly. default 0, 1 to turn it on. Note: The skill will give additional lethality anyway based on the base damage output.
