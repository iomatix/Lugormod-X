# Extern Variables List
**They are usable inside server.cfg file.**

List of all Extern Variables added by iomatix:

### LMD:

- **`lmd_jedi_pickup_weapons`** 1 is default. 
  
  1 allows Force User to pick up weapons and ammo from the map. Also adds light saber drop mechanic. Change to 0 to set old lugor-style.
- **`lmd_jedi_add_hp_level`** 1 is default. 
  
  Allows Force User to extend HP capacity per level, 0 is blocking that feature.  
- **`lmd_profession_fee`** 1350 is default. 
  
  It's Credits cost of the profession change. If credits value is smaller than this value it will take all CR belonging to the player.
- **`lmd_exp_for_level`** 450 is default. 
  
  This value is used to calculate how many experience points player will need to gain the next level.
- **`lmd_skillpoint_cost`** 485 is default. 
  
  It's Credits cost of spending one skill point for upgrading skills. 
- **`lmd_bounty_streaks_by`** 5 is default. 
  
  That value is used in killing streak counter. After that value killing streak reward for killing the player is increasing.
- **`lmd_skillpoints_perlevel`** 4 is default. 

  That value determines how many skill points player will receive after each level-up. After reaching the Mastery Level the value slightly increasing. 
- **`lmd_mastery_level_skills_level`**
  
  That value determines max level of the skills before reaching Mastery Level. 3 is default. 
- **`lmd_damage_level_scale`** 
When is set to 1 the damage output will be scaled with level difference between attacker and target (Anti-grief way). 0 to turn off, 1 is default.

- **`lmd_is_lethality_add_damage`** 
When is set to 1 the damage output based on lethality skill will increase, with 0 the damage will be converted instead. default 0, 1 to turn it on.

- **`lmd_is_thousandcuts_lethality`** 
When is set to 1 the additional damage output is converted to the lethality, subtracting it from HP directly. default 0, 1 to turn it on. Note: The skill will give additional lethality anyway based on the base damage output.

- **`lmd_rewardexp_kill`** & **`lmd_rewardcr_kill`**
Set-up base rewards for frags. Default 45 & 35. The rewards depend on the level differences between players.

- **`lmd_is_differences_formula`**
Choose the gain formula. 1 is based on difference between players. 0 is normal constant formula.

- **`lmd_is_buy_level`**
When 1 players get access to buylevel command. Default 0.

- **`lmd_stashexp`** & **`lmd_stashcr`**
Allow to setup amount experience and credits inside stash.
- **`lmd_startingCr`**

Starting credits value for new registered account. Default 1350.
- **`lmd_old_commands_disp`**

Hides new commands from the chat, turn them back to the console log only if 1. If 2 most messages will appear on the chat. Defalut 0. (1 is Old lugor-style)
- **`lmd_is_forcecooldown`**

Adds cooldown for Force User's force powers if 1. Avoid spamming forces. Default 1. (0 to turn off the cooldown feature)
- **`lmd_force_is_double_jump`**

Allows to turn off the double jump if 0. Default 1.
- **`lmd_bots_gain_experience`**

Allows to gain experience and The Black List rewards for AI players if 1. Default 0.
### G:

- **`g_scaleAllDamag`**

Allows to change total damage scaling.

### Targets:

**They are usable in the game**

- **`target_experience`** and **`count_exp`**. It stores experience data.
New target entity target_experience with count_exp variable. EXAMPLE: place `target_experience |0| |targetname|,|experience|,count_exp,|200|`
