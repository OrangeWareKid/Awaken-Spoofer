#pragma once
#include "includes.h"

using namespace KeyAuth;

std::string name = skCrypt("cracke").decrypt(); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = skCrypt("nM0KX4mAyq").decrypt(); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = skCrypt("df751312e8869ce0e5977ccd7e4e19c8e5ba85d965e831a2bd3f1f7b7a0fe6ef").decrypt(); // app secret, the blurred text on licenses tab and other tabs
std::string version = skCrypt("1.0").decrypt(); // leave alone unless you've changed version on website
std::string url = skCrypt("https://keyauth.win/api/1.2/").decrypt(); // change if you're self-hosting

api KeyAuthApp(name, ownerid, secret, version, url);

class c_globals {
public:
	bool active = true;

	char username[128];
	char password[128];
	HWND hwnd;

	ImFont* Arial;
	ImFont* PTRootUIBold;
	ImFont* PTRootUIBoldLarge;


	//Spoofer
	bool DiscEmulator = true;
	bool DiscEmulator1 = true;
	bool DiscEmulator2 = true;
	bool DiscEmulator3 = true;
	bool DiscEmulator4 = false;
	bool DiscEmulator5 = false;

	//Cleaner
	bool FortniteClean = true;
	bool WarzoneClean = false;
	bool EFTClean = false;


	bool temploaded = true;
	bool permloaded = true;


	enum pages
	{
		init,
		login,
		dashboard,
		spoof,
		loading,

	};
	pages tab{ init };

	enum subtabs
	{
		Dashboard,
		Utilities,
		Cheats,
		msgs,
		info
		

	};
	subtabs subtabs{ Dashboard };
};

inline c_globals globals;