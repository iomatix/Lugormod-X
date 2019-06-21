# The Commands List
- **`help`** and **`help |command|`** to show more detailed information about commands available.
- **`clear`** to clear the console log.
- **`connect |IP|`** to connect to the server directly using IPv4.
- **`kill`** self command to kill player's character. Usable when someone is stuck or want to reset active flags.

## Account Commands
- **`help account`** to print all commands.
- **`alias |new_name|`** to change in-game name. The user name will not change so your login data remains the same.
- **`chpasswd |new_password|`** to change the account's password.
- **`credits`** to check current wealth.
- **`creditbox open`** to open the Credit-Box.
- **`dropcr |credits|`** to drop |credits|CR out.
- **`inventory list|use|destroy`** to manage player's inventory.
- **`login |username| |password|`**, **`login |username| |password| |seccode|`** to log in to an existing account.
- **`logout`** to log out the account.
- **`pay |credits|`** give |credits|CR to the player you are looking at.
- **`property list|setrank|removeadded`** to manage properties.
- **`register |username| |password|`** to register a new account.
- **`seccode`** to check account's security code.
- **`stats`** to check detailed account's information.
- **`worthy`** to list players worth dueling.

## Player Commands
- **`help player`** to print all commands.
- **`actions`** to list current pending actions.
- **`admins`** to list currently logged in admins and their auth level.
- **`bounty |name| |credits|`** to place an order to the Black List.
- **`blacklist`** and **`blacklist |name|`** to check the Black List.
- **`buddy`** and **`buddy |name|`** to check your buddies list or add the player to the list.
- **`challenge |type|`** to challenge someone to a special duel. types: power, force, fullforce, training, tiny, titan, bet, hibet.
- **`chatmode say|team |chatmode|`** to switch the say or team chat to the special chat. chatmodes: All, Team, Admins, Buddies, Friends
- **`confirm yes|no|toggle`** to confirm an action or toggle the confirmation requirement.
- **`drinkme`** 
- **`drophi`** to drop active holdable item out.
- **`dropjp`** to drop the jetpack out.
- **`dropstash`** to drop the money stash out.
- **`dropwp`** to drop active weapon out.
- **`eatme`**
- **`emote`** to play an animation.
- **`examine`** to examine the object in front of the player.
- **`factions`** view and interact with factions. Factions are creatable only for admin with **`factionadmin`** command.
- **`friends list|add|remove`** to check your friends list or add|remove the player from to the list.
- **`hilevel`** to Display Top 10 players with the highest level on the server.
- **`hikills`** to Display Top 10 players with the most kills on the server.
- **`hiscore`** to Display Top 10 players with the highest score on the server.
- **`histashes`** to Display Top 10 players with the most stashes on the server.
- **`hitime`** to Display Top 10 players with the most time spend on the server.
- **`ignore |player|`** to ignore messages. **`ignore -1`** to ignore all the messages.
- **`interact`** to interact with terminals and objects.
- **`ionlyduel`** to set onlyduel flag. Can not receive or deal damage except duels. Removable after **`kill`** command or engagion a duel.
- **`king`** to get information about present Duel King on the server.
- **`motd`** to display server's MOTD.
- **`ragequit`** to quit from server without saving.
- **`say_buddies`** to send message to buddies from the buddies list.
- **`say_close`** to send a message to those standing close the player.
- **`stash`** to get information about money stashes on the server.

## Profession Commands
- **`help profession`** to print all commands.
- **`buylevel confirm`** to buy a level in current profession if it is enabled on the server. (This option is disabled by default.)
- **`profession`** and **`profession |name|`** to change the profession.
- **`resetskill`**, **`resetskill confirm`** to reset all active skills.
- **`skill`**, **`skill |name|`** and **`skill |name| up`** to check detailed information concerning profession's skills. **`up`** to upgrade the skill.
- **`newgame start`** resets current profession's progress. Above the Mastery Level in return increases New Game Plus Level which gives New Game Plus Skill Points. 
- **`newgame reset`** resets the New Game Plus Level.

### Mercenary Commands
- **`cortosis`** to equip an armor which turns off hostile lightsabers and lowers splash damage. Can not use splash weapons. (Exclusive command for Mercenaries only.)
- **`flame`** to launch a spew of flames. It has the same bind as force Lightning - setable in the control settings. (Exclusive command for the Flame Skill owners only.) 
- **`weapons |weapon|`** to toggle starting weapons. (Exclusive command for the Weapons Skill owners only.) 
- **`ysalamiri`** to use ysalamiri. It has the same bind as the challenge to duel button - setable in the control settings. (Exclusive command for the Ysalamiri Skill owners only.) 

### Force User Commands
- **`ionlysaber`** to set onlysaber flag. Can not receive or deal damage thorugh the force. Removable after **`kill`** command. (Exclusive command for Force Users only.)


## Admin Commands
- **`help admin`** to print all commands
- **`grantadmin setrank |username| |auth_rank|`** to register an existing user as an admin. 1 is the most privileged rank. (Server's console only.)

### Addon: [External Variables List](https://github.com/iomatix/Lugormod-X/blob/master/__example_config/externvars.md)
### Addon: [External Entities List](http://adamo.uw.hu/entities.html) + target_experience with count_exp












