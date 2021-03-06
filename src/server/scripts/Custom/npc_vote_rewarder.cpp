/*
* Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2014 Roca <http://wow-studio.eu/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Standard Copyrightgezuppel, kannst du ruhig entfernen, wenn du moechtest
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

enum VoteRewarder
{
                     //Hier beliebige entry aus der Tabelle npc_text eintragen. Momentan halt standard Text :)
    GOSSIP_MESSAGE = DEFAULT_GOSSIP_MESSAGE,

    MAIN_MENU  = 1,
    MENU_60P   = 2,
    MENU_250P  = 3,
    MENU_500P  = 4,
    MENU_1000P = 5,
    MENU_2000P = 6,
    MENU_MISC  = 7,
};

class npc_vote_rewarder : public CreatureScript
{
public:
    npc_vote_rewarder() : CreatureScript("npc_vote_rewarder") { }

    uint32 votePoints = 0;
    uint32 accId = 0;

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        accId = player->GetSession()->GetAccountId();

        QueryResult result = LoginDatabase.PQuery("SELECT points FROM voting_points WHERE id = %u", accId);

        if (result)
        {
            Field* fields = result->Fetch();
            votePoints = fields[0].GetUInt32();
        }

        player->ADD_GOSSIP_ITEM(7, "Votepunkte eintauschen.", MAIN_MENU, 1);
        player->ADD_GOSSIP_ITEM(0, "Bis bald.",               MENU_MISC, 1);
        player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == MAIN_MENU)
        {
            player->ADD_GOSSIP_ITEM(7, "Belohnungen f\303\274r 60 Punkte.",   MENU_60P,   1);
            player->ADD_GOSSIP_ITEM(7, "Belohnungen f\303\274r 250 Punkte.",  MENU_250P,  1);
            player->ADD_GOSSIP_ITEM(7, "Belohnungen f\303\274r 500 Punkte.",  MENU_500P,  1);
            player->ADD_GOSSIP_ITEM(7, "Belohnungen f\303\274r 1000 Punkte.", MENU_1000P, 1);
            player->ADD_GOSSIP_ITEM(7, "Belohnungen f\303\274r 2000 Punkte.", MENU_2000P, 1);
            player->ADD_GOSSIP_ITEM(0, "Bis bald.",                           MENU_MISC,  1);
            player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
        }

        if (sender == MENU_60P)
        {
            switch (action)
            {
                case 1:
                    player->ADD_GOSSIP_ITEM(2, "60 Punkte gegen 100 Gold tauschen.", MENU_60P,  2);
                    player->ADD_GOSSIP_ITEM(7, "Hauptmen\303\274.",                  MAIN_MENU, 1);
                    player->ADD_GOSSIP_ITEM(0, "Bis bald.",                          MENU_MISC, 1);
                    player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
                    break;
                case 2:
                    if (votePoints < 60)
                    {
                        creature->Whisper("Du hast nicht gen\303\274gend Votepunkte", LANG_UNIVERSAL, player);
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }

                    if ((player->GetMoney() + (100 * GOLD)) > MAX_MONEY_AMOUNT)
                    {
                        creature->Whisper("Du kannst so viel Gold nicht bei dir tragen.", LANG_UNIVERSAL, player);
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }
                    else
                    {
                        player->ModifyMoney(100 * GOLD);
                        LoginDatabase.PExecute("UPDATE voting_points SET points = (points - %u) WHERE id = %u;", 60, accId);
                        player->CLOSE_GOSSIP_MENU();
                    }
                    break;
            }
        }

        if (sender == MENU_250P)
        {
            switch (action)
            {
                case 1:
                    player->ADD_GOSSIP_ITEM(2, "250 Punkte gegen 10x Emblem des Heldentums tauschen.", MENU_250P, 2);
                    player->ADD_GOSSIP_ITEM(2, "250 Punkte gegen 10x Emblem der Eroberung tauschen.",  MENU_250P, 3);
                    player->ADD_GOSSIP_ITEM(2, "250 Punkte gegen 10x Emblem der Ehre tauschen.",       MENU_250P, 4);
                    player->ADD_GOSSIP_ITEM(2, "250 Punkte gegen 10x Emblem des Triumphs tauschen.",   MENU_250P, 5);
                    player->ADD_GOSSIP_ITEM(2, "250 Punkte gegen 10x Emblem des Frosts tauschen.",     MENU_250P, 6);
                    player->ADD_GOSSIP_ITEM(7, "Hauptmen\303\274.",                                    MAIN_MENU, 1);
                    player->ADD_GOSSIP_ITEM(0, "Bis bald.",                                            MENU_MISC, 1);
                    player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
                    break;
                case 2:
                    SendVotereward(player, 40752, 10, 250);
                    break;
                case 3:
                    SendVotereward(player, 45624, 10, 250);
                    break;
                case 4:
                    SendVotereward(player, 40753, 10, 250);
                    break;
                case 5:
                    SendVotereward(player, 47241, 10, 250);
                    break;
                case 6:
                    SendVotereward(player, 49426, 10, 250);
                    break;
            }
        }

        if (sender == MENU_500P)
        {
            switch (action)
            {
                case 1:
                    player->ADD_GOSSIP_ITEM(2, "500 Punkte gegen 1000 Gold tauschen.",           MENU_500P, 2);
                    player->ADD_GOSSIP_ITEM(2, "500 Punkte gegen Katzentransportkorb tauschen.", MENU_500P, 3);
                    player->ADD_GOSSIP_ITEM(7, "Hauptmen\303\274.",                              MAIN_MENU, 1);
                    player->ADD_GOSSIP_ITEM(0, "Bis bald.",                                      MENU_MISC, 1);
                    player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
                    break;
                case 2:
                    if (votePoints < 500)
                    {
                        creature->Whisper("Du hast nicht gen\303\274gend Votepunkte", LANG_UNIVERSAL, player);
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }

                    if ((player->GetMoney() + (1000 * GOLD)) > MAX_MONEY_AMOUNT)
                    {
                        creature->Whisper("Du kannst so viel Gold nicht bei dir tragen.", LANG_UNIVERSAL, player);
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }
                    else
                    {
                        player->ModifyMoney(1000 * GOLD);
                        LoginDatabase.PExecute("UPDATE voting_points SET points = (points - %u) WHERE id = %u;", 500, accId);
                        player->CLOSE_GOSSIP_MENU();
                    }
                    break;
                case 3:
                    SendVotereward(player, 8491, 1, 500);
                    break;
            }
        }

        if (sender == MENU_1000P)
        {
            switch (action)
            {
                case 1:
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen XT der Kleine tauschen.",         MENU_1000P,  2);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Pandarenm\303\266nch tauschen.",  MENU_1000P,  3);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Hippogryphenjunges tauschen.",    MENU_1000P,  4);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Papierdrachen tauschen.",         MENU_1000P,  5);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Papageienk\303\244fig tauschen.", MENU_1000P,  6);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Tuskarrdrachen tauschen.",        MENU_1000P,  7);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Eingefangene Flamme tauschen.",   MENU_1000P,  8);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Wunschwettermaschine tauschen.",  MENU_1000P,  9);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Fetisch aus Sen´jin tauschen.",   MENU_1000P, 10);
                    player->ADD_GOSSIP_ITEM(2, "1000 Punkte gegen Makabre Marionette tauschen.",    MENU_1000P, 11);
                    player->ADD_GOSSIP_ITEM(7, "Hauptmen\303\274.",                                 MAIN_MENU,   1);
                    player->ADD_GOSSIP_ITEM(0, "Bis bald.",                                         MENU_MISC,   1);
                    player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
                    break;
                case 2:
                    SendVotereward(player, 54847, 1, 1000);
                    break;
                case 3:
                    SendVotereward(player, 49665, 1, 1000);
                    break;
                case 4:
                    SendVotereward(player, 23713, 1, 1000);
                    break;
                case 5:
                    SendVotereward(player, 34493, 10, 1000);
                    break;
                case 6:
                    SendVotereward(player, 8494, 1, 1000);
                    break;
                case 7:
                    SendVotereward(player, 49287, 1, 1000);
                    break;
                case 8:
                    SendVotereward(player, 23083, 1, 1000);
                    break;
                case 9:
                    SendVotereward(player, 35227, 1, 1000);
                    break;
                case 10:
                    SendVotereward(player, 45606, 1, 1000);
                    break;
                case 11:
                    SendVotereward(player, 46831, 1, 1000);
                    break;
            }
        }

        if (sender == MENU_2000P)
        {
            switch (action)
            {
                case 1:
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Z\303\274gel des schnellen Spektraltiger tauschen.",               MENU_2000P,  2);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Z\303\274gel des wei\303\237en Eisb\303\244ren tauschen.",         MENU_2000P,  3);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Z\303\274gel des gro\303\237en schwarzen Kriegsmammuts tauschen.", MENU_2000P,  4);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Chopper/Feuerstuhl tauschen.",                                     MENU_2000P,  5);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Schneller Netherdrachen tauschen.",                                MENU_2000P,  6);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Netherdrachen des Erbarmungslosen tauschen.",                      MENU_2000P,  7);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Rachs\303\274chtiger Netherdrachen tauschen.",                     MENU_2000P,  8);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Brutaler Netherdrachen tauschen.",                                 MENU_2000P,  9);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Frostwyrm des t\303\266dlichen Gladiators tauschen.",              MENU_2000P, 10);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Reitschildkr\303\266te tauschen.",                                 MENU_2000P, 11);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Himmelsross tauschen.",                                            MENU_2000P, 12);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen Flammender Hippogryph tauschen.",                                  MENU_2000P, 13);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen X-51 Netherrakete tauschen.",                                      MENU_2000P, 14);
                    player->ADD_GOSSIP_ITEM(2, "2000 Punkte gegen X-51 Netherrakete X-Trem tauschen.",                               MENU_2000P, 15);
                    player->ADD_GOSSIP_ITEM(7, "Hauptmen\303\274.",                                                                  MAIN_MENU,   1);
                    player->ADD_GOSSIP_ITEM(0, "Bis bald.",                                                                          MENU_MISC,   1);
                    player->PlayerTalkClass->SendGossipMenu(GOSSIP_MESSAGE, creature->GetGUID());
                    break;
                case 2:
                    SendVotereward(player, 49284, 1, 2000);
                    break;
                case 3:
                    SendVotereward(player, 43962, 1, 2000);
                    break;
                case 4:
                    SendVotereward(player, player->GetTeam() == ALLIANCE ? 43959 : 44083, 1, 2000);
                    break;
                case 5:
                    SendVotereward(player, player->GetTeam() == ALLIANCE ? 44413 : 41508, 1, 2000);
                    break;
                case 6:
                    SendVotereward(player, 30609, 1, 2000);
                    break;
                case 7:
                    SendVotereward(player, 34092, 1, 2000);
                    break;
                case 8:
                    SendVotereward(player, 37676, 1, 2000);
                    break;
                case 9:
                    SendVotereward(player, 43516, 1, 2000);
                    break;
                case 10:
                    SendVotereward(player, 46171, 1, 2000);
                    break;
                case 11:
                    SendVotereward(player, 23720, 1, 2000);
                    break;
                case 12:
                    SendVotereward(player, 54811, 1, 2000);
                    break;
                case 13:
                    SendVotereward(player, 54069, 1, 2000);
                    break;
                case 14:
                    SendVotereward(player, 49285, 1, 2000);
                    break;
                case 15:
                    SendVotereward(player, 49286, 1, 2000);
                    break;
            }
        }

        if (sender == MENU_MISC)
        {
            if (action == 1)
            {
                creature->Whisper("Bis bald.", LANG_UNIVERSAL, player);
                player->CLOSE_GOSSIP_MENU();
            }
        }

        return true;
    }

    void SendVotereward(Player* player, uint32 itemEntry, uint32 count, uint32 vp)
    {
        if (votePoints < vp)
        {
            player->Whisper("Du hast nicht gen\303\274gend Votepunkte", LANG_UNIVERSAL, player);
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        uint32 noSpaceForCount = 0;

        ItemPosCountVec dest;
        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, count, &noSpaceForCount);

        //Der Spieler bekommt die Belohnung via IngameMail, wenn er keinen Platz mehr in seinen Taschen hat.
        if (msg != EQUIP_ERR_OK)
        {
            //Texte kannst du aendern, wie du lustig bist.
            std::string subject = "Votebelohnung";
            std::string text = "Hier deine Belohnung. Vielen dank f\303\274r deine Unterst\303\274tzung. Bitte vote auch in zukunft f\303\274r unseren Server.";

            typedef std::pair<uint32, uint32> ItemPair;
            typedef std::list< ItemPair > ItemPairs;
            ItemPairs items;

            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(itemEntry);

            while (count > item_proto->GetMaxStackSize())
            {
                items.push_back(ItemPair(itemEntry, item_proto->GetMaxStackSize()));
                count -= item_proto->GetMaxStackSize();
            }

            items.push_back(ItemPair(itemEntry, count));

            if (items.size() > MAX_MAIL_ITEMS)
                return;

            MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);
            MailDraft draft(subject, text);

            SQLTransaction trans = CharacterDatabase.BeginTransaction();

            for (ItemPairs::const_iterator itr = items.begin(); itr != items.end(); ++itr)
            {
                if (Item* item = Item::CreateItem(itr->first, itr->second, 0))
                {
                    item->SaveToDB(trans);
                    draft.AddItem(item);
                }
            }

            draft.SendMailTo(trans, MailReceiver(player, ObjectGuid(player->GetGUID())), sender);
            CharacterDatabase.CommitTransaction(trans);
        }
        else
        {
            player->AddItem(itemEntry, count);
        }

        LoginDatabase.PExecute("UPDATE voting_points SET points = (points - %u) WHERE id = %u;", vp, accId);

        player->Whisper("Hier deine Belohnung. Vielen dank f\303\274r deine Unterst\303\274tzung. Bitte vote auch in zukunft f\303\274r unseren Server.", LANG_UNIVERSAL, player);
    }
};

void AddSC_npc_vote_rewarder()
{
    new npc_vote_rewarder();
}
