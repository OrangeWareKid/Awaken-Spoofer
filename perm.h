#pragma once
#include "includes.h"
#include "changer.h"
#include "driver.h"
#include "lazy.h"
#include "xorstr.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <string>
#include <ctime>
#include <cstdlib>
bool CreateFileFromMemory(const std::string& desired_file_path, const char* address, size_t size)
{
	std::ofstream file_ofstream(desired_file_path.c_str(), std::ios_base::out | std::ios_base::binary);

	if (!file_ofstream.write(address, size))
	{
		file_ofstream.close();
		return false;
	}

	file_ofstream.close();
	return true;
}

bool ReadFileToMemory(const std::string& file_path, std::vector<uint8_t>* out_buffer)
{
	std::ifstream file_ifstream(file_path, std::ios::binary);

	if (!file_ifstream)
		return false;

	out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close();

	return true;
}

void perm()
{
	auto chapath = xorstr("C:\\Windows\\System32\\zhjers.exe");
	auto drvpath = xorstr("C:\\Windows\\System32\\AMIFLDRV64.SYS");

	CreateFileFromMemory(chapath, reinterpret_cast<const char*>(Changer), sizeof(Changer));
	CreateFileFromMemory(drvpath, reinterpret_cast<const char*>(Driver), sizeof(Driver));

	system("C:\\Windows\\System32\\zhjers.exe /SU auto");
	system("C:\\Windows\\System32\\zhjers.exe /SS \"To be filled by O.E.M\" ");
	system("C:\\Windows\\System32\\zhjers.exe /BS \"%random%%random%\" ");
	system("C:\\Windows\\System32\\zhjers.exe /PSN \"Unknown\" ");
	system("C:\\Windows\\System32\\zhjers.exe /CS \"To be filled by O.E.M\" ");
	system("C:\\Windows\\System32\\zhjers.exe /SK \"To be filled by O.E.M\" ");
	system("C:\\Windows\\System32\\zhjers.exe /SF \"To be filled by O.E.M.\" ");
	system("C:\\Windows\\System32\\zhjers.exe /BT \"To be filled by O.E.M.\" ");
	system("C:\\Windows\\System32\\zhjers.exe /BLC \"To be filled by O.E.M.\" ");
	system("C:\\Windows\\System32\\zhjers.exe /CA \"To be filled by O.E.M.\" ");
	system("C:\\Windows\\System32\\zhjers.exe /CSK \"To be filled by O.E.M.\" ");
	system("C:\\Windows\\System32\\zhjers.exe /IVN \"American Megatrends International, LLC.\" ");
	system("C:\\Windows\\System32\\zhjers.exe /ID 06/27/23 ");
	system("C:\\Windows\\System32\\zhjers.exe /IV \"A.F0\" ");;

	remove("C:\Windows\System32\zhjers.exe");
	remove("C:\Windows\System32\AMIFLDRV64.SYS");


}