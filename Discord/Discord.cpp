#include "Discord.h"
#include <time.h>
#include <iostream>
#include <chrono>
static int64_t eptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();


void Discord::Initialize()
{
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize("1164758097227677696", &Handle, 1, NULL); //Your Api Key
}

void Discord::Update()
{
	//Copy Your Api Pre-Visualize
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = "#1 Multi Client";
	discordPresence.startTimestamp = eptime;

	discordPresence.largeImageKey = "logo";
	discordPresence.largeImageText = "logo";
	discordPresence.smallImageKey = "logo";
	discordPresence.smallImageText = "dsc.gg/KXHA8EQanH";

	// 1st btn
	discordPresence.button1Label = "Download";
	discordPresence.button1Url = "";
	// 2scnd btn
	discordPresence.button2Label = "Discord";
	discordPresence.button2Url = "https://discord.gg/KXHA8EQanH";

	// //discordPresence.largeImageKey = "vector-initial-d-letter-logo-with-swoosh-colored-red-and-black_2_"; //large image file name no extension
	// discordPresence.largeImageKey = "combined_screenshot"; //large image file name no extension
	// discordPresence.largeImageText = "dsc.gg/drivt";
	// //discordPresence.smallImageKey = "1769-shockly"; //same as large
	// discordPresence.smallImageKey = "wallpaperbetter_com_1920x1080"; //same as large
	// discordPresence.smallImageText = "dsc.gg/driv"; //displays on hover
	Discord_UpdatePresence(&discordPresence);
}