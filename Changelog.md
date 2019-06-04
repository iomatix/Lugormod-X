# Changelog:

## 3.0.1.0
- New Lugormod X version by iomatix release. ✔️
- Registration reworked. ✔️
- Bot's logging reworked. They are able to use professions now. ✔️
- Bot is trying to re-log on spawn when it's not logged. ✔️
- Profession selection is changed. ✔️
- Changed Maximum level up to 120. ✔️
- Admin is able to change level up to 255 via command now. Remember that, max buyable level is 120! ✔️
- After reaching 40 level Force user is able to learn new skill tree. ✔️
- Force Power adjusted depending on profession level. ✔️
- Skillreset command bug is fixed. ✔️
- Mercenary gets his jetpack after first fuel skill upgrade. ✔️
- Added lmd_jedi_pickup_weapons, set it to 1 to allow jedi class to pick up weapons and ammo from the map. Default 0 ✔️
- Added lmd_jedi_add_hp_level, set it to 1 to allow jedi class to get additional health-points per level. Default 1 ✔️
- Added lmd_profession_fee variable to set the fee when changing the profession. Default 1350. ✔️
- Added lmd_skillpoints_perlevel variable that amount of skill points will be gived per each level up. Default 4. ✔️
- Reworked skillpoints checking system. Old one was totally bugged. ✔️
- Added more basic information in command-popup and repaired old messy ones for usable commands. ✔️
- Health, Force Power and Shield formula is re-balanced. ✔️
- When meditating Force Power raises 20% faster.  ✔️
- Meditation formula re-balanced.  ✔️
- Player can spend skillpoints to upgrade their max hp/power/shield capacity. ✔️
- Added new skill tree for Jedi and new passive skills. ✔️
- lmd_skillpoint_cost will have impact also on reset skill cost. Reseting skill is the same price as uprgrading. ✔️
- Redesigned skill creation for mercenary. ✔️
- Mercenary weapons skill has access to more weapons now. ✔️
- Skills above lmd_mastery_level_skills_level level availble after reaching the Mastery Level (40). ✔️ default 3, to unlock use 99 or 0. ✔️
- Flameburst increased damage output. ✔️ 
- Added level damage scaling with new server side variable lmd_damage_level_scale. default 1, 0 to turn off ✔️ 
- Added lethality & thousand cuts skills. ✔️ 
- Added new extern variable lmd_is_lethality_add_damage to configure the lethality skill. default 0, 1 to turn it on. ✔️ 
- Added new extern variable lmd_is_thousandcuts_lethality to configure the thousand cuts skill. default 0, 1 to turn it on. ✔️ 
- Added new passive skill Master of The Guns for Mercenery. ✔️ 
- Added new extern variable lmd_rewardexp_kill to set-up the reward for kill. ✔️ 
- Added new extern variable lmd_exp_for_level. ✔️ 
- Added new extern variable lmd_is_buy_level, set to 1 get access to the buylevel command. default 0 ✔️ 
- True RPG leveling system is implemented. ✔️ 
- Now admin commands work also on two arguments, without name argument it means self as an account argument. ✔️
- Meditation bug is fixed. ✔️
- lmd_is_differences_formula is added which allows to choose between: credits and experience gain depends on level difference between players if 1, or constant values for each level if 0.  ✔️
- New admin commands implemented. ✔️
- Current level synchronizes with data-based one. ✔️
- Credit-boxes are added. Gain them by leveling up. ✔️
- New Game Plus implemented. Gain more skillpoints with it. ✔️
- Skills system is connected to economy. Manage it with credits. ✔️
- Changeable aliases (nicknames) added. ✔️
- Stats panel re-designed.  ✔️
- Lightnings turn off the jetpack for a while.  ✔️
- Rage drains health from hurted enemies. ✔️
- Added target_experience and count_exp for quests it allows to gain experience as a reward for doing quests. ✔️
- The title system. ✔️
- Lightning memory leak fixed. ✔️
- Skillpoints cost default is 350 now. ✔️
- Main skills need more points to upgrade. ✔️
- lmd_skillpoints_perlevel 5 by default.  ✔️
- AI memory leaks fixed. GetIdealDestination in StandardBotAI ✔️
- Creditboxes reworked. ✔️
- Player must confirm resetskills command to reset skills now. ✔️
- Head damage is increased. ✔️
- Perfect Aim mercenary skill is added. ✔️
- Bounty system with rewards is added. ✔️
- Bounty command is added now players are able to set the new bounties. ✔️ 
- Profession Commands Reworked. ✔️
- Account Commands Rework. ✔️
- Player Commands Rework. ✔️
- Highest killstreak statistic is added. ✔️
- Bounties list called The Black List. ✔️
- Bryar pistol added to weapon skill. Mercenary starts without a weapon. ✔️
- Player is able to drop saber or pistol on the ground if lmd_jedi_pickup_weapons is 1. ✔️
- Force is able to control sabers on the ground. ✔️
- Added simple AI to watch the Black List and manage it. ✔️
- Mercenary bots start with weapons depending on their weapons skill level. ✔️
- Rage life-steal re-worked. When active, up to 35% lifesteal when is no-active up to 16%. ✔️

///
- jedi is able to pick up medpacks ???
- lmd_old_commands_disp
- duel rewards
- deflecting and parring with saber costs power.
- Search a new ways for spending credits.
- experience boosters/ packs for CR? lmd_exp_boosters
- Level 5 Jump. The player can not use it to fly but jumps higher. ??
- lmd_stashexp added. Stash system supports new experience system now. 50%
- Sound Banks references updated for KotF. ?% (Thermal,DetPack,TripMine) something is not okay there!!!!!!!
- Longer Grip which depends on the skill level. 80% Search for limiter function... whereveri it is hidden.
- Longer Drain/Heal cooldowns or heal rework to work overtime not an instan.
- Cooldowns system, when cooldown force drains more force. 



## TODO

- grip longer

- Try repair crosschair hud for force.  (CG_PLAYER_ARMOR_VALUE  CG_PLAYER_AMMO_VALUE   CG_PLAYER_FORCE_VALUE)
- flsaber check saber model "let's change the person hp and force to 1 and broadcast message "x is a lame cuz trying use a invisible light sabers"
- TODO: Add drop item from inventory.  todo: drop (+recall), sell (drop+buyoption + recall) inventory_core.c

- TODO: svcmds + commands + admin commands guide

- TODO: Trading system + droping items. Add message trap_SendServerCommand
- TODO: Messages motd for duel types. trap_SendServerCommand

- TODO: New styles (god is using them, for force user) Cmd_SaberAttackCycle_f ??

- TODO: USER FRIENDLY GUIDE
- TODO: Skills documented with commands belove.
- TODO: Adding openjk API support
- TODO: RGB lightsabers
- TODO: Custom swords
- TODO: New lightning effects for level 4 & 5
