function checkpanel() {
	new playerid, giverid, actionid, actiontime, complaint, reason[64], dm, name[25], givername[25], reason2[64];
	gString[0] = (EOS);
	format(gString, 256, "SELECT * FROM `panelactions`");
	new Cache: stringresult = mysql_query(SQL, gString);

	if(cache_get_row_count() > 0) {
		for(new i, j = cache_get_row_count(); i != j; ++i) {
			playerid 					= cache_get_field_content_int(i, "playerid");
			giverid 					= cache_get_field_content_int(i, "giverid");
			actionid					= cache_get_field_content_int(i, "actionid");
			complaint 					= cache_get_field_content_int(i, "complaintid");
			dm 							= cache_get_field_content_int(i, "dm");
			actiontime					= cache_get_field_content_int(i, "actiontime");
			cache_get_field_content(i, "playername", name, SQL, 25);
			cache_get_field_content(i, "givername", givername, SQL, 25);
			cache_get_field_content(i, "reason", reason, SQL, 64);	

			switch(actionid) {
				case 1: {
					gString[0] = (EOS);
					format(gString, 256, "AdmPanel: %s has been banned by %s for %d days, reason: %s.", name, givername, actiontime, reason);
					SCMTA(COLOR_LIGHTRED, gString);
					
					mysql_format(SQL, gString, sizeof gString, "INSERT INTO bans (Name, AdminName, Reason, Days, Permanent, Active) VALUES('%s', '%s', '%s', '%d', 0, 1)", name, givername, reason2, actiontime);
					mysql_tquery(SQL, gString, "", "");	
					mysql_format(SQL, gString, 256, "INSERT INTO punishlogs (`playerid`,`giverid`,`actionid`,`actiontime`,`reason`,`playername`,`givername`) VALUES ('%d','%d','1','%d','reclamatie','%s','%s')", playerid, giverid, actiontime, 0, name, givername);
					mysql_tquery(SQL, gString, "", "");
					mysql_format(SQL, gString, 256, "UPDATE `users` SET `Ban` = '1', `BanDays` = '%d' WHERE `Name` = '%s' LIMIT 1", actiontime, name);
					mysql_tquery(SQL, gString, "", ""); 
					foreach(new idd : Player) if(playerVariables[idd][pSQLID] == playerid) defer KickEx(idd);
				}
				case 2: {
					gString[0] = (EOS);
					format(gString, 256, "AdmPanel: %s has been permanent banned by %s, reason: %s.", name, givername, reason);
					SCMTA(COLOR_LIGHTRED, gString);
					
					mysql_format(SQL, gString, sizeof gString, "INSERT INTO bans (Name, AdminName, Reason, Days, Permanent, Active) VALUES('%s', '%s', '%s', '999', 1, 1)", name, givername, reason2);
					mysql_tquery(SQL, gString, "", "");	
					mysql_format(SQL, gString, 256, "INSERT INTO punishlogs (`playerid`,`giverid`,`actionid`,`actiontime`,`reason`,`playername`,`givername`) VALUES ('%d','%d','2','0','reclamatie','%s','%s')", playerid, giverid, name, givername);
					mysql_tquery(SQL, gString, "", "");
					mysql_format(SQL, gString, 256, "UPDATE `users` SET `Ban` = '1', `BanDays` = '999' WHERE `Name` = '%s' LIMIT 1", name);
					mysql_tquery(SQL, gString, "", "");
					foreach(new idd : Player) if(playerVariables[idd][pSQLID] == playerid) defer KickEx(idd);
				}
				case 3: {
					gString[0] = (EOS);
					if(complaint > 0) {
						format(gString, 256, "AdmPanel: %s was jailed by %s for %d minutes, reason: %s [complaint: #%d].", name, givername, actiontime, reason, complaint);
						SCMTA(COLOR_LIGHTRED, gString);
						format(reason2, 64, "%s [complaint: %d]", reason, complaint);
					}
					mysql_format(SQL, gString, 256, "UPDATE `users` SET `Jailed` = '1', `JailTime`= '%d' WHERE `Name` = '%s' LIMIT 1", actiontime*60, name);
					mysql_tquery(SQL, gString, "", "");
					mysql_format(SQL, gString, 256, "INSERT INTO punishlogs (`playerid`,`giverid`,`actionid`,`actiontime`,`reason`,`playername`,`givername`) VALUES ('%d','%d','6','%d','%s','%s','%s')", playerid, giverid, actiontime, reason2, name, givername);
					mysql_tquery(SQL, gString, "", "");
					foreach(new idd : Player) {
						if(playerVariables[idd][pSQLID] == playerid) {
							ResetWeapons(idd);
							WantedPoints[idd] = 0; 
							playerVariables[idd][pJailed] = 2; 
							playerVariables[idd][pJailTime] = actiontime*60;
							PutPlayerInAJail(idd);
							if(dm > 0) { playerVariables[idd][pGunLic] = 0; playerVariables[idd][pGunLicT] = 0; playerVariables[idd][pGunLicSuspend] = dm + 1; }
						}
					}
				}
				case 4: {
					gString[0] = (EOS);
					format(gString, 256, "AdmPanel: %s received a warn from admin %s, reason %s. ", name, givername, reason);
					SCMTA(COLOR_LIGHTRED, gString);
					
					mysql_format(SQL, gString, 256, "UPDATE `users` SET `Warnings` = `Warnings` + '1' WHERE `Name` = '%s' LIMIT 1", name);
					mysql_tquery(SQL, gString, "", "");
					mysql_format(SQL, gString, 256, "INSERT INTO punishlogs (`playerid`,`giverid`,`actionid`,`actiontime`,`playername`,`givername`) VALUES ('%d','%d','5','%d','reclamatie','%s','%s')", playerid, giverid, actiontime, name, givername);
					mysql_tquery(SQL, gString, "", "");
					foreach(new idd : Player) if(playerVariables[idd][pSQLID] == playerid) { playerVariables[idd][pWarns] ++; defer KickEx(idd); } 
				}
				case 5: {
					format(gString, 256, "AdmPanel: %s has been muted by %s for %d minutes, reason: %s.", name, givername, actiontime, reason);
					SCMTA(COLOR_LIGHTRED, gString);
					
					mysql_format(SQL, gString, 256, "UPDATE `users` SET `Muted` = '1', `MuteTime` = '%d' WHERE `Name` = '%s' LIMIT 1", actiontime*60, name);
					mysql_tquery(SQL, gString, "", "");
					mysql_format(SQL, gString, 256, "INSERT INTO punishlogs (`playerid`,`giverid`,`actionid`,`actiontime`,`reason`,`playername`,`givername`) VALUES ('%d','%d','7','%d','reclamatie','%s','%s')", playerid, giverid, actiontime, name, givername);
					mysql_tquery(SQL, gString, "", "");
					foreach(new idd : Player) if(playerVariables[idd][pSQLID] == playerid) { playerVariables[idd][pMuteTime] = actiontime*60; }
				}
				case 6: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): %s [user: %d] received a warn from administrator %s [user: %d] (profile action).", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 7: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] has unmuted %s [user: %d] from panel.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new idd : Player)
					{
						if(playerVariables[idd][pSQLID] == playerid)
						{
							playerVariables[idd][pMuteTime] = 0;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s has unmuted you.", givername);
							SCM(idd, COLOR_RED, gQuery);
						}
					}
				}
				case 8: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] added %s [user: %d] to premium users.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
			
					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pPremiumAccount] = 1;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s added you to premium users.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 9: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] added %s [user: %d] to vip users.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pVIP] = 1;
							
							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s added you to vip users.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 10: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] added %s [user: %d] to golden users.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pGoldenAccount] = 1;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s added you to golden users.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 11: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] added vehicle slot to %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pCarSlots] ++;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s added you slot vehicle.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 12: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] removed %s [user: %d] from helpers team.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pHelper] = 0;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s removed you from helpers team.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 13: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] added %s [user: %d] to beta tester team.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 14: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] removed %s [user: %d] from admins team.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pAdmin] = 0;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s removed you from admins team.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 15:
				{
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] has unbanned %s [user: %d] from profile.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 16:
				{
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] reseted faction application for %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 17: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] created a new vehicle for %s [user: %d] from panel.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s created a new vehicle for you (please relog).", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 18: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] changed nickname %s [user: %d] from panel.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
					
					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s changed nickname you.", givername);
							SCM(x, COLOR_RED, gQuery);

							defer KickEx(x);
						}
					}
				}
				case 19: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] cleared a warn for %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
					
					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pWarns] --;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s cleared a warn for you.", givername);
							SCM(x, COLOR_RED, gQuery);
						}
					}	
				}
				case 20: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] setted premium points to %d for %s [user: %d].", 1, givername, giverid, actiontime, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pPremiumPoints] = actiontime;

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s setted premium points to %d for you.", givername, actiontime, name);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 21: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] setted level to %d for %s [user: %d].", 1, givername, giverid, actiontime, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);

					foreach(new x : Player)
					{
						if(playerVariables[x][pSQLID] == playerid)
						{
							playerVariables[x][pLevel] = actiontime;

							SetPlayerScore(x, actiontime);

							gQuery[0] = (EOS);
							format(gQuery, 256, "(panel): administrator %s setted level to %d for you.", givername, actiontime, name);
							SCM(x, COLOR_RED, gQuery);
						}
					}
				}
				case 22: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] setted hours for %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 23: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] changed email for %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 24: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] setted money for %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 25: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] reseted parameter profile for %s [user: %d].", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 26: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] gaved %s [user: %d] suspend from panel.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
				case 27: {
					gString[0] = (EOS);
					format(gString, 256, "(panel): administrator %s [user: %d] gaved %s [user: %d] a staff warn.", 1, givername, giverid, name, playerid);
					SendAdminMessage(COLOR_RED, gString, 1);
				}
			}
		}		
		new query[150];
		mysql_format(SQL, query, sizeof query, "DELETE FROM `panelactions` WHERE `actionid` = '%d'", actionid);
		mysql_tquery(SQL, query, "", "");
	}	
	cache_delete(stringresult);
	if(mysql_unprocessed_queries() >= 3) SendAdminMessage(COLOR_RED, "(panel): too many queries requests, can't execute actions.", 1);	
	return true;
}
La FiveSecondsTimer jos de tot sub return 1 adaugi checkpanel();