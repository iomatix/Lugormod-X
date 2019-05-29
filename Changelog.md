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
- When meditating Force Power rains 20% faster.  ✔️
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
- Sound Banks references updated for KotF. ?% (Thermal,DetPack,TripMine) something is not okay there!!!!!!!
- Added new extern variable lmd_rewardexp_kill to set-up the reward for kill. ✔️ 
- Added new extern variable lmd_exp_for_level. ✔️ 
- Added new extern variable lmd_is_buy_level, set to 1 get access to the buylevel command. default 0 ✔️ 
- True RPG leveling system is implemented. ✔️ 
- Now admin commands work also on two arguments, without name argument it means self as an account argument. ✔️
- Meditation bug is fixed. ✔️
- lmd_is_differences_formula is added which allows to choose between: credits and experience gain depends on level difference between players if 1, or constant values for each level if 0.  ✔️
- lmd_stashexp added. Stash system supports new experience system now. ✔️
- New admin commands implemented.
- Current level synchronizes with data-based one.
- Credit-boxes is added.
- New Game Plus implemented. Gain more skillpoints with it.
- Titles system.
- Skills system is connected to economy. Manage it with credits.
- Changeable aliases (nicknames) added.

## TODO
- skills cost hotfix!
- flsaber check saber model "let's change the person hp and force to 1 and broadcast message "x is a lame cuz trying use a invisible light sabers"
- Titles in stats display depends on level + new game plus points. Add section in stats command 
- lootboxes for levels
- save profession levels. changing profession cost adds skills reset cost. , new game plus vars, lootboxes one var 55%,25%, 15%, 4% , 1%. (buffor size capacity loading files limit)
- Change profession checking to auth while buylevel cmd.
- Merclevel/jedilevel update on levelup (buylevel and experience ways)
- Add new game plus start user command. Add benefits from it. (prof core.c todo) !!!
add everything to stats command


- ammo pickup event bug fix if smaller magasine than normal //engine limitation?
- smaller ammo amounts
- adjust explosives amounts

- stash has exp
- quests exp //if i'll learn it

- TODO: Now skillpoints will cost credits to be spend (level(level+1)lmd_skillpoint_cost)  //default 150


- TODO: Player after reaching level 120 is able to start over again from level 1 but with perma +25hp/+25force/+25shield boost. Repeatable. 
- TODO: Killing streak Bounty. After each lmd_bounty_streaks_by kills bounty is increasing. For killing bounty target player will recive Credit Boxes. new variable (lmd_bounty_streaks_by default "5")

- TODO: Repair memory leaks with AI say command

- TODO: svcmds + commands + admin commands guide

- TODO: Lootbox for level up each 10 lvls %10
- TODO: Messages time control.
- TODO: Add drop item from inventory.
- TODO: Repair selling option trading system. Add message (motd) + try to console disp. If not create new buy/sell inv options.
- TODO: Messages motd for duel types.
- TODO: Lootboxes for killstreak

- TODO: Lightning turn off jetpacks for a while. 

- TODO: New styles (god is using them, for force user)

- TODO: USER FRIENDLY GUIDE
- TODO: Adding openjk API support
- TODO: RGB lightsabers
- TODO: Custom swords
- TODO: New lightning effects for level 4 & 5
