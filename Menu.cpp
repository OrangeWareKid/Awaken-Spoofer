#include "includes.h"
#include "imgui_notify.h"
#include <thread>  
#include <Windows.h> 
#include "perm.h"

void move_window()
{
    GetWindowRect(hwnd, &rc);
    if (ImGui::GetWindowPos().x != 0 || ImGui::GetWindowPos().y != 0)
    {
        MoveWindow(hwnd, rc.left + ImGui::GetWindowPos().x, rc.top + ImGui::GetWindowPos().y, MenuSize.x, MenuSize.y, TRUE);
        ImGui::SetWindowPos(ImVec2(0, 0));
    }
}
std::string fileid;


static std::string msg_text;


bool HasPermanent = true;
bool HasTemp = true;

bool HasFortniteLite = true;
bool HasFortnitePremium = true;
bool HasFortniteRage = true;
bool HasValorant = true;
bool HasModernWarfare2 = true;
bool HasFIVEM = true;




bool BaseBoard = true;
bool Memory = true;
IDirect3DTexture9* g_pTexture = nullptr;



static bool navbar_collapsed = false;
float navbar_length = 3.f;
void login()
{
    navbar_collapsed = false;
    Sleep(3000);
    KeyAuthApp.login(globals.username, globals.password);
    if (KeyAuthApp.data.success)
    {
        UpdatePrefFile();

        globals.tab = globals.dashboard;
        navbar_length = 1.f;
        
        navbar_collapsed = true;

        for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
            auto sub = KeyAuthApp.data.subscriptions.at(i);
            {
                if (sub.name == "Temporary")
                {
                    std::time_t now = std::time(0);
                    std::string expiry_str = sub.expiry;
                    std::time_t expiry;

                    try {
                        expiry = std::stol(expiry_str);
                    }
                    catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                    }
                    catch (const std::out_of_range& e) {
                        std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                    }

                    std::time_t diff = expiry - now;
                    int days = diff / (60 * 60 * 24);

                    std::string Expiry = std::to_string(days) + " day(s)";

                    std::cout << Expiry << std::endl;
                }
            }
        }
    }
    if (!KeyAuthApp.data.success)
    {
        navbar_collapsed = true;
        Sleep(500);
  /*      ImGui::InsertNotification({ ImGuiToastType_Success, 4000, "Successfully loaded." });*/
        ImGui::InsertNotification({ ImGuiToastType_Error, 4000, KeyAuthApp.data.message.c_str()});
    }

    
}

void load(std::string fileid)
{
    navbar_length = 3.f;
    globals.tab = globals.loading;
    navbar_collapsed = !navbar_collapsed;
    
    Sleep(3000);
    globals.tab = globals.dashboard;
    //std::vector<std::uint8_t> bytes = KeyAuthApp.download(fileid);
  /*  PYTGuard_MapFile();*/
    navbar_collapsed = !navbar_collapsed;
    navbar_length = 1.f;
    Beep(300, 300);
    std::cout << fileid;
}
bool subtab;

enum heads {
    dashboard, Utilities, cheats, settings, skins, configs, scripts, temporary, permanent
};

const char* items[] = { "Option 1", "Option 2", "Option 3" };
int currentItem = 0;


static heads tab{ dashboard };

DWORD WINAPI loadWrapper(LPVOID lpParameter) {
    // Cast lpParameter to the appropriate type
    const char* data = (const char*)lpParameter;

    // Call the load function with the string parameter
    load(data);

    return 0;
}

const char* tab_name = tab == dashboard ? "Dashboard" : tab == Utilities ? "Utilities" : tab == cheats ? "Cheats" : tab == settings ? "Settings" : tab == skins ? "Skins" : tab == configs ? "Configs" : tab == scripts ? "Scripts" : 0;

void initialize()
{
    navbar_collapsed = false;
    name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear();

    AutoUpdate au;								// For auto updating
    au.CheckForUpdates();
   /* Sleep(3000);*/
    navbar_collapsed = true;
    globals.tab = globals.dashboard;
}

ImVec2 navbar_Size;

int ImDialogBoxBase(ImVec2 ps)
{
    if (!msg_text[0])
        return 0;

    ImGui::SetNextWindowPos(ImVec2{ImGui::GetWindowSize().x - 360, ImGui::GetWindowSize().y - 144});
    ImGui::SetNextWindowSize({ 360.f , 144.f });

    ImGui::Begin(skCrypt("unload_window").decrypt(), nullptr, ImGuiWindowFlags_NoDecoration);


    ImVec2 p = ImGui::GetWindowPos();
    auto draw_list = ImGui::GetWindowDrawList();

 
    draw_list->AddRectFilled(p, p + ImVec2(800, 500), ImColor(0.05f, 0.05f, 0.05f, 0.9f));

    draw_list->AddRectFilled({ p.x, p.y + 74.f }, { p.x + 360.f, p.y + 144.f }, ImColor(31, 32, 34, 50), 0.f);

    draw_list->AddText({ p.x + 16.f, p.y + 16.f }, ImColor(234, 238, 245, 255),
        skCrypt("Notification").decrypt());

    draw_list->AddText({ p.x + 16.f, p.y + 39.f }, ImColor(179, 183, 195, 255),
        msg_text.c_str());

    ImGui::SetCursorPos({ 16.f, 86.f });

    if (ImGui::Button(skCrypt("Okay").decrypt(), { 330.f, 42.f })) {
        msg_text = "";
    }

    ImGui::End();
}


void ui::render() {
    if (!globals.active) return;



    static float navbar_length = 3.f;

    if (globals.tab == globals.dashboard)
    {
        navbar_length = 1.f;
    }
    static float navbar_width = 0.f; navbar_width = ImLerp(navbar_width, navbar_collapsed ? 0.f : navbar_length, 0.04f);
    content_anim = ImLerp(content_anim, 1.f, 0.008f);

    ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Loader", &globals.active, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
        ImGui::RenderNotifications();
        ImGui::PopStyleVar(1); // Don't forget to Pop()
        ImGui::PopStyleColor(1);

        move_window();
        Particles();

        switch (globals.tab)
        {
        case globals.init: {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !navbar_collapsed);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_anim * (navbar_width > 0.005f ? 1 / navbar_width / 2 : 1.f));
            static int init = false;
            if (!init) {
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)initialize, NULL, NULL, NULL);
                init = true;
            }

            ImGuiIO& io = ImGui::GetIO();
            ImDrawList* overlayDrawList = ImGui::GetForegroundDrawList();
            ImVec2 textSize = ImGui::CalcTextSize(VersionString);
            ImVec2 textPosition = ImVec2(io.DisplaySize.x - textSize.x - 5, io.DisplaySize.y - textSize.y);
            overlayDrawList->AddText(textPosition, IM_COL32(128, 128, 128, 255), VersionString);

            ImGui::PopStyleVar();
            ImGui::PopItemFlag();
            break;
        }
        case globals.login: {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !navbar_collapsed);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_anim * (navbar_width > 0.005f ? 1 / navbar_width / 2 : 1.f));

            //ImGui::SetCursorPos({ CalculateXOffsetForCentering(64), 60 });
            //ImGui::Image(g_pTexture, ImVec2(64, 64));

            ImGui::PushFont(globals.PTRootUIBoldLarge);
            ImGui::SetCursorPos({ CalculateXOffsetForCenteringText("Awaken"), 85});
            ImGui::TextColored(ImVec4(ImColor(56, 101, 242)), "Awaken");
            
            ImGui::PopFont();

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::SetCursorPos({ CalculateXOffsetForCentering(220), ImGui::GetCursorPosY() });
            ImGui::InputTextEx(" Username", NULL, globals.username, 64, ImVec2(220, 40), 0);
            ImGui::SetCursorPos({CalculateXOffsetForCentering(220), ImGui::GetCursorPosY() });
            ImGui::InputTextEx(" Password", NULL, globals.password, 64, ImVec2(220, 40), ImGuiInputTextFlags_Password);
            ImGui::SetCursorPos({CalculateXOffsetForCentering(220), ImGui::GetCursorPosY() });
            if (ImGui::Button("Login", { 220, 40 }))
            {
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)login, NULL, NULL, NULL);

            }
 

            ImGuiIO& io = ImGui::GetIO();
            ImDrawList* overlayDrawList = ImGui::GetForegroundDrawList();
            ImVec2 textSize = ImGui::CalcTextSize(VersionString);
            ImVec2 textPosition = ImVec2(io.DisplaySize.x - textSize.x - 5, io.DisplaySize.y - textSize.y);
            overlayDrawList->AddText(textPosition, IM_COL32(128, 128, 128, 255), VersionString);

            ImGui::PopStyleVar();
            ImGui::PopItemFlag();
            break;
        }
        case globals.dashboard: {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !navbar_collapsed);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_anim * (navbar_width > 0.005f ? 1 / navbar_width / 2 : 1.f));

            if (subtabs == globals.Dashboard)
            {
                ImGui::SetCursorPos({0, 0 });
                ImGui::BeginChild("Content", { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});
                {

                    for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                        auto sub = KeyAuthApp.data.subscriptions.at(i);
                        {
                            if (sub.name == "permanent")
                                HasPermanent = true;

                            if (sub.name == "temporary")
                                HasTemp = true;
                        }
                    }


                    ImGui::SetCursorPosX(225);
                    if (ImGuiCustom::subtab("Temporary", spoofertabs == temporary)) { spoofertabs = temporary; }
                    ImGui::SameLine();

                    if (ImGuiCustom::subtab("Permanent", spoofertabs == permanent)) { spoofertabs = permanent; }
                    ImGui::SameLine();

                    if (ImGuiCustom::subtab("Configs", spoofertabs == configs)) { spoofertabs = configs; }



                    if (spoofertabs == permanent)
                    {
                      
                            ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - 500) / 2 + 30, 70 });
                            ImGuiCustom::begin_child("Information", { 500, 315 });
                            {
                                ImGui::Indent(10);
                                ImGui::PushFont(globals.PTRootUIBoldLarge);
                                ImGui::Text("Permanent Spoofer");
                                ImGui::PopFont();

                                ImGui::Separator();

                                ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                                /*       ImGui::SameLine(440.f);
                                       ImGui::Image((void*)g_pTexture, ImVec2(52.0f, 52.0f));*/




                                ImGui::TextDisabled("Updated > 02/13/2024");
                                for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                    auto sub = KeyAuthApp.data.subscriptions.at(i);
                                    {
                                        if (sub.name == "Permanent")
                                        {
                                            std::time_t now = std::time(0);
                                            std::string expiry_str = sub.expiry;
                                            std::time_t expiry;

                                            try {
                                                expiry = std::stol(expiry_str);
                                            }
                                            catch (const std::invalid_argument& e) {
                                                std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                            }
                                            catch (const std::out_of_range& e) {
                                                std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                            }

                                            std::time_t diff = expiry - now;
                                            int days = diff / (60 * 60 * 24);

                                            std::string Expiry = std::to_string(days) + " day(s)";

                                            ImGui::TextDisabled(Expiry.c_str());
                                        }
                                    }
                                }
                                ImGui::SetCursorPosY(200);
                                if (!globals.permloaded)
                                {
                                    if (ImGui::Button("Spoof", { 235, 30 }))
                                    {
                                        globals.permloaded = true;

                                        SetWindowRgn(hwnd, NULL, TRUE);
                                        ::ShowWindow(hwnd, SW_MAXIMIZE);

                                        ::UpdateWindow(hwnd);
                                        globals.tab = globals.spoof;

                                        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)perm, NULL, NULL, NULL);
                                    }
                                }
                                if (globals.permloaded)
                                {
                                    if (ImGui::Button("Spoof", { 235, 30 }))
                                    {
                                        
                                    }
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Serial Checker", { 235, 30 }))
                                {

                                    const char* fileid = "511305";
                                    CreateThread(NULL, 0, [](LPVOID lpParameter) -> DWORD {
                                        const char* fileid = static_cast<const char*>(lpParameter);
                                        load(fileid);
                                        return 0;
                                        }, (LPVOID)fileid, 0, NULL);


                                }
                                if (ImGui::Button("Restore your old HardwareID", { 480, 30 }))
                                {

                                    const char* fileid = "511305";
                                    CreateThread(NULL, 0, [](LPVOID lpParameter) -> DWORD {
                                        const char* fileid = static_cast<const char*>(lpParameter);
                                        load(fileid);
                                        return 0;
                                        }, (LPVOID)fileid, 0, NULL);


                                }
                            } ImGuiCustom::end_child();
                    }
                    if (spoofertabs == temporary)
                    {
                        ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - 500) / 2 + 30, 70 });
                        ImGuiCustom::begin_child("Information", { 500, 315 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Temporary Spoofer");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("Updated > 02/13/2024");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "Temporary")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }
                            ImGui::SetCursorPosY(200);

                            if (!globals.temploaded)
                            {
                                if (ImGui::Button("Spoof", { 235, 30 }))
                                {

                                    globals.temploaded = true;

                                    SetWindowRgn(hwnd, NULL, TRUE);
                                    ::ShowWindow(hwnd, SW_MAXIMIZE);
                                    ::UpdateWindow(hwnd);
                                    globals.tab = globals.spoof;

                                    std::thread([=]() {

                                        std::this_thread::sleep_for(std::chrono::seconds(3));

                                        runpe::ProcessHollow("120123");

                                        if (globals.DiscEmulator) // randomizes gpu
                                        {
                                            Beep(600, 900);
                                        }
                                        std::this_thread::sleep_for(std::chrono::seconds(3));

                                        if (globals.DiscEmulator2) // randomizes uuid
                                        {
                                            Beep(600, 900);
                                        }
                                        std::this_thread::sleep_for(std::chrono::seconds(3));

                                        if (globals.DiscEmulator3) // Randomizes Network
                                        {
                                            Beep(600, 900);
                                        }

                                        std::this_thread::sleep_for(std::chrono::seconds(3));
                                        if (globals.DiscEmulator4) // Cleaner
                                        {
                                            Beep(600, 900);

                                        }



                                        std::this_thread::sleep_for(std::chrono::seconds(10));


                                        Beep(600, 900);
                                        MessageBox(NULL, L"Successfully Executed Driver", L"Notifications", MB_OK);

                                        }).detach();
                                }
                            }
                            if (globals.temploaded)
                            {
                
                            }



                            ImGui::SameLine();
                            if (ImGui::Button("Deep Clean", { 235, 30 }))
                            {

                                const char* fileid = "511305";
                                CreateThread(NULL, 0, [](LPVOID lpParameter) -> DWORD {
                                    const char* fileid = static_cast<const char*>(lpParameter);
                                    load(fileid);
                                    return 0;
                                    }, (LPVOID)fileid, 0, NULL);


                            }
                            if (ImGui::Button("Restore your old HardwareID", { 480, 30 }))
                            {

                                const char* fileid = "511305";
                                CreateThread(NULL, 0, [](LPVOID lpParameter) -> DWORD {
                                    const char* fileid = static_cast<const char*>(lpParameter);
                                    load(fileid);
                                    return 0;
                                    }, (LPVOID)fileid, 0, NULL);




                            }

                        } ImGuiCustom::end_child();
                    }
                
                    if (spoofertabs == configs)
                    {
                        ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - 500) / 2 + 30, 70 });
                        ImGuiCustom::begin_child("Information", { 500, 315 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Configuration");
                            ImGui::PopFont();

                            ImGui::Separator();
                            ImGui::Checkbox("Randomizes Disks", &globals.DiscEmulator);
                            ImGui::Checkbox("Randomizes GPU", &globals.DiscEmulator2);
                            ImGui::Checkbox("Randomizes UUID", &globals.DiscEmulator3);
                            ImGui::Separator();
                            ImGui::Checkbox("Clean Fortnite", &globals.FortniteClean);
                            ImGui::Checkbox("Clean Warzone", &globals.WarzoneClean);
                            //ImGui::Checkbox("Clean Escape from Tarkov", &globals.EFTClean);
                            
                        } ImGuiCustom::end_child();
                        
                    }

                    

                    

                }
                ImGui::EndChild();

               
            }
            if (subtabs == globals.Utilities)
            {
                ImGui::SetCursorPos({0, 0 });
                ImGui::BeginChild("Content", { 700, 0 });
                {
                    ImGui::Indent(150);
                    ImGuiCustom::begin_child("Windows Activator", { 515, 100 });
                    {
                        ImGui::Indent(10);
                        ImGui::TextWrapped("A Windows activator is a tool that enables users \nto activate Windows without a valid product key");

                        ImGui::SetCursorPos({ 310, 5 });
                        ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Working");
                        ImGui::SetCursorPos({ 415, 5 });
                        ImGui::TextDisabled("Updated > 02/13/2024");

                        ImGui::SetCursorPos({ 310, 25 });
                            if (ImGui::Button("Run", { 205, 30 }))
                        {

                        }
                    } ImGuiCustom::end_child();
          
                    ImGuiCustom::begin_child("Windows Corruption Repair", { 515, 100 });
                    {
                        ImGui::Indent(10);
                        ImGui::TextWrapped("Windows corruption repair is a process to fix \nissues caused by damaged system files, ensuring \nsystem stability.");

                        ImGui::SetCursorPos({ 310, 5 });
                        ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Working");
                        ImGui::SetCursorPos({ 415, 5 });
                        ImGui::TextDisabled("Updated > 02/13/2024");

                        ImGui::SetCursorPos({ 310, 25 });
                            if (ImGui::Button("Run", { 205, 30 }))
                        {

                        }
                    } ImGuiCustom::end_child();
        
                    ImGuiCustom::begin_child("WMI Repair", { 515, 100 });
                    {
                        ImGui::Indent(10);
                        ImGui::TextWrapped("WMI repair addresses issues with the \nWindows Management Instrumentation service, \ncrucial for system management.");

                        ImGui::SetCursorPos({ 310, 5 });
                        ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Working");
                        ImGui::SetCursorPos({ 415, 5 });
                        ImGui::TextDisabled("2/14/2024");

                        ImGui::SetCursorPos({ 310, 25 });
                            if (ImGui::Button("Run", { 205, 30 }))
                        {
                                
                        }
                    } ImGuiCustom::end_child();
         
                    ImGuiCustom::begin_child("Network Reset", { 515, 100 });
                    {
                        ImGui::Indent(10);
                        ImGui::TextWrapped("Network reset in Windows reverts all \nnetwork settings to default, helping resolve \nconnectivity issues.");

                        ImGui::SetCursorPos({ 310, 5 });
                        ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Working");
                        ImGui::SetCursorPos({ 415, 5 });
                        ImGui::TextDisabled("Updated > 02/13/2024");

                        ImGui::SetCursorPos({ 310, 25 });
                            if (ImGui::Button("Run", { 205, 30 }))
                        {

                        }
                    } ImGuiCustom::end_child();
  
                  
                }
                ImGui::EndChild();
                


                /*ImGui::SetCursorPos({ 385, 45 });
                ImGuiCustom::begin_child("WMI Repair", { 100, 360 });
                {
                    ImGui::Indent(10);


                    ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                    ImGui::TextDisabled("11/30/2023");
                    ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                    if (ImGui::Button("Run", { 180, 30 }))
                    {
                        elapsedTime = 0.0f;
                        SetWindowRgn(hwnd, NULL, TRUE);
                        ::ShowWindow(hwnd, SW_MAXIMIZE);

                        ::UpdateWindow(hwnd);
                        globals.tab = globals.spoofing;
                    }
                } ImGuiCustom::end_child();*/
            }
            if (subtabs == globals.Cheats)
            {
                ImGui::SetCursorPos({ navbar_Size.x + 30, 0 });
                ImGui::BeginChild("Content", { 0, 455 }, ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_HorizontalScrollbar);
                {
                    for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                        auto sub = KeyAuthApp.data.subscriptions.at(i);
                        {
                            if (sub.name == "FortniteLite")
                                HasFortniteLite = true;

                            if (sub.name == "FortnitePremium")
                                HasFortnitePremium = true;
                            if (sub.name == "FortniteRage")
                                HasFortniteRage = true;

                            if (sub.name == "Valorant")
                                HasValorant = true;

                            if (sub.name == "ModernWarfare2")
                                HasModernWarfare2 = true;

                            if (sub.name == "Fivem")
                                HasFIVEM = true;
                        }
                    }

                    // oh wait 

                    if (HasFortnitePremium)
                    {
                        ImGui::SetCursorPos({ 5, 30 });

                        ImGuiCustom::begin_child("Fortnite Premium", { 225, 400 });
                        {

                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Fortnite Premium");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "FortnitePremium")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }



                            ImGui::TextDisabled("Version: ");  // Render the first part with the original color
                            ImGui::SameLine();                 // Continue on the same line
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f)); // Set the color to light blue
                            ImGui::Text("1.1.1");              // Render the colored part
                            ImGui::PopStyleColor();


                            ImGui::TextWrapped("1. Press load sUEFI (ONE-TIME)\n\n 2. Load Initialize sEFI. \n\n 3. Press Load Cheat In Lobby.\n");
                            if (ImGui::Button("Testtt", { 205, 30 }))
                            {
                                Beep(600, 900);

                                runpe::ProcessHollow("261202");


                            }
                            if (ImGui::Button("Initialize sEFI", { 205, 30 }))
                            {
                                Beep(600, 900);

                                runpe::ProcessHollow("773190"); 

                               

                            }
                            if (ImGui::Button("Load Cheat", { 205, 30 }))
                            {
                                Beep(600, 900);

                                runpe::ProcessHollow("618107");
                            }
                        } ImGuiCustom::end_child();
                    }
                    else
                    {
                        ImGui::SetCursorPos({ 5, 30 });

                        ImGuiCustom::begin_child("Fortnite Premium", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Fortnite Premium");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("NO ACTIVE SUBSCRIPTION");


                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("PURCHASE", { 205, 30 }))
                            {
                                ShellExecute(0, 0, L"https://awakencheats.com", 0, 0, SW_SHOW);
                            }
                        } ImGuiCustom::end_child();
                    }

                    
                        if (HasFortniteLite)
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

             
                        ImGuiCustom::begin_child("Fortnite Lite", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Fortnite Lite");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "FortniteLite")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("Run", { 205, 30 }))
                            {
                           
                            }
                        } ImGuiCustom::end_child();
                    }
                    else
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);



                        ImGuiCustom::begin_child("Fortnite Lite", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Fortnite Lite");
                            ImGui::PopFont();



                    

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("NO ACTIVE SUBSCRIPTION");
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("PURCHASE", { 205, 30 }))
                            {
                                ShellExecute(0, 0, L"https://awakencheats.com", 0, 0, SW_SHOW);
                            }
                        } ImGuiCustom::end_child();
                    }
                   
                    if (HasFortniteRage)
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("Fortnite Rage", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Fortnite Rage");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "FortniteRage")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("Run", { 205, 30 }))
                            {

                            }
                        } ImGuiCustom::end_child();
                    }
                    else
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("Fortnite Rage", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Fortnite Rage");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("NO ACTIVE SUBSCRIPTION");
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("PURCHASE", { 205, 30 }))
                            {
                                ShellExecute(0, 0, L"https://awakencheats.com", 0, 0, SW_SHOW);
                            }
                        } ImGuiCustom::end_child();
                    }
                   
                    if (HasValorant)
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("Valorant", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Valorant");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "Valorant")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("Run", { 205, 30 }))
                            {

                            }
                        } ImGuiCustom::end_child();
                    }
                    else
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("Valorant", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Valorant");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("NO ACTIVE SUBSCRIPTION");
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("PURCHASE", { 205, 30 }))
                            {
                                ShellExecute(0, 0, L"https://awakencheats.com", 0, 0, SW_SHOW);
                            }
                        } ImGuiCustom::end_child();
                    }
                    
                    if (HasModernWarfare2)
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("Modern Warfare 2", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("ModernWarfare2");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "ModernWarfare2")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("Run", { 205, 30 }))
                            {

                            }
                        } ImGuiCustom::end_child();
                    }
                    else
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("Modern Warfare 2", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("Modern Warfare 2");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("NO ACTIVE SUBSCRIPTION");
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("PURCHASE", { 205, 30 }))
                            {
                                ShellExecute(0, 0, L"https://awakencheats.com", 0, 0, SW_SHOW);
                            }
                        } ImGuiCustom::end_child();
                    }
                   
                    if (HasFIVEM)
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("FIVEM", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("FIVEM");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                auto sub = KeyAuthApp.data.subscriptions.at(i);
                                {
                                    if (sub.name == "FIVEM")
                                    {
                                        std::time_t now = std::time(0);
                                        std::string expiry_str = sub.expiry;
                                        std::time_t expiry;

                                        try {
                                            expiry = std::stol(expiry_str);
                                        }
                                        catch (const std::invalid_argument& e) {
                                            std::cerr << "Invalid input for expiry: " << e.what() << std::endl;
                                        }
                                        catch (const std::out_of_range& e) {
                                            std::cerr << "Out of range input for expiry: " << e.what() << std::endl;
                                        }

                                        std::time_t diff = expiry - now;
                                        int days = diff / (60 * 60 * 24);

                                        std::string Expiry = std::to_string(days) + " day(s)";

                                        ImGui::TextDisabled(Expiry.c_str());
                                    }
                                }
                            }
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("Run", { 205, 30 }))
                            {

                            }
                        } ImGuiCustom::end_child();
                    }
                    else
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(30);

                        ImGuiCustom::begin_child("FIVEM", { 225, 400 });
                        {
                            ImGui::Indent(10);
                            ImGui::PushFont(globals.PTRootUIBoldLarge);
                            ImGui::Text("FIVEM");
                            ImGui::PopFont();

                            ImGui::Separator();


                            ImGui::TextColored(ImVec4(ImColor(13, 230, 0)), "Undetected");
                            ImGui::TextDisabled("NO ACTIVE SUBSCRIPTION");
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
                            if (ImGui::Button("PURCHASE", { 205, 30 }))
                            {
                                ShellExecute(0, 0, L"https://awakencheats.com", 0, 0, SW_SHOW);
                            }
                        } ImGuiCustom::end_child();
                    }
                    

                    ImGui::SameLine();
                    ImGui::SetCursorPosY(30);

                    ImGui::Dummy({200, 500});
                }
                ImGui::EndChild();
            }
            
            ImGui::PopStyleVar();
            ImGui::PopItemFlag();
            break;
        }
        case globals.spoof: {
            ui::spoofing();
            break;
        }
       
        }

        ImGui::SetCursorPos({ 0, 0 });

        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetWindowPos();

        if (globals.tab != globals.dashboard)
        {
            ImGui::BeginChild("navbar", { 50 + 220 * navbar_width, window->Size.y }, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse);
            {

                ImGui::GetWindowDrawList()->AddRectFilled(p, p + ImGui::GetWindowSize(), ImColor(0, 0, 0), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);


                ImVec2 childSize = ImGui::GetContentRegionAvail();
                if (globals.tab == globals.dashboard)
                {
                    ImGui::GetWindowDrawList()->AddRectFilled(p, p + ImGui::GetWindowSize(), ImColor(22,  22, 22), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);
                    ImGui::GetWindowDrawList()->AddRectFilled({ p.x + ImGui::GetWindowWidth() - 1, p.y }, p + ImGui::GetWindowSize(), ImGui::GetColorU32(ImGuiCol_Border));
                    ImGui::SetCursorPosX(childSize.x / 2.0f - 27.5f);
                    ImGui::Image((void*)g_pTexture, ImVec2(52.0f, 52.0f));
                }
                ImGui::SetCursorPosY(65);

                ImVec2 windowSize = ImGui::GetWindowSize();
                ImVec2 imageSize(32, 32);
                ImVec2 imagePos((windowSize.x - imageSize.x) * 0.5f, (windowSize.y - imageSize.y - 16) * 0.5f);


                if (globals.tab == globals.login)
                {
                    if (!navbar_collapsed)
                    {


                        float spinnerWidth = 10.0f; // Set to the desired width of the spinner
                        float spinnerHeight = 10.0f * 3; // Assuming bars = 3

                        // Get the window size
                        float windowWidth = ImGui::GetWindowSize().x;
                        float windowHeight = ImGui::GetWindowSize().y;

                        // Calculate the horizontal and vertical positions to center the spinner
                        float posX = (windowWidth - spinnerWidth) * 0.5f;
                        float posY = (windowHeight - spinnerHeight) * 0.5f;

                        // Set the cursor position to center the spinner
                        ImGui::SetCursorPos(ImVec2(posX - 16.5, posY + 30));
                        ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(63, 92, 165), ImSpinner::Speed(6.8f), 3, true);
                    }
                }

                if (globals.tab == globals.init)
                {
                    if (!navbar_collapsed)
                    {


                        float spinnerWidth = 10.0f; // Set to the desired width of the spinner
                        float spinnerHeight = 10.0f * 3; // Assuming bars = 3

                        // Get the window size
                        float windowWidth = ImGui::GetWindowSize().x;
                        float windowHeight = ImGui::GetWindowSize().y;

                        // Calculate the horizontal and vertical positions to center the spinner
                        float posX = (windowWidth - spinnerWidth) * 0.5f;
                        float posY = (windowHeight - spinnerHeight) * 0.5f;

                        // Set the cursor position to center the spinner
                        ImGui::SetCursorPos(ImVec2(posX - 16.5, posY + 30));
                        ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(63, 92, 165), ImSpinner::Speed(6.8f), 3, true);
                    }
                }
            }
            ImGui::EndChild();
        }
        if (globals.tab == globals.loading)
        {
            ImGui::BeginChild("navbar", { 50 + 220 * navbar_width, window->Size.y }, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse);
            {

                ImGui::GetWindowDrawList()->AddRectFilled(p, p + ImGui::GetWindowSize(), ImColor(0, 0, 0), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);

                float spinnerWidth = 10.0f; // Set to the desired width of the spinner
                float spinnerHeight = 10.0f * 3; // Assuming bars = 3

                // Get the window size
                float windowWidth = ImGui::GetWindowSize().x;
                float windowHeight = ImGui::GetWindowSize().y;

                // Calculate the horizontal and vertical positions to center the spinner
                float posX = (windowWidth - spinnerWidth) * 0.5f;
                float posY = (windowHeight - spinnerHeight) * 0.5f;

                // Set the cursor position to center the spinner
                ImGui::SetCursorPos(ImVec2(posX - 16.5, posY + 30));
                ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(63, 92, 165), ImSpinner::Speed(6.8f), 3, true);



             
            }
            ImGui::EndChild();
        }
        if (globals.tab == globals.dashboard)
        {
            ImGui::BeginChild("navbar", { 50 + 110 * navbar_width, window->Size.y }, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse);
            {
                navbar_Size = ImGui::GetContentRegionAvail();
                ImGui::GetWindowDrawList()->AddRectFilled(p, p + ImGui::GetWindowSize(), ImColor(225, 33, 40, 255), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);


                ImVec2 childSize = ImGui::GetContentRegionAvail();
                if (globals.tab == globals.dashboard)
                {
                    ImGui::GetWindowDrawList()->AddRectFilled(p, p + ImGui::GetWindowSize(), ImColor(0, 0, 0), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);
                    ImGui::GetWindowDrawList()->AddRectFilled({ p.x + ImGui::GetWindowWidth() - 1, p.y }, p + ImGui::GetWindowSize(), ImGui::GetColorU32(ImGuiCol_Border));

                    if (!navbar_collapsed)
                    {
                        ImGui::PushFont(globals.PTRootUIBoldLarge);
                        ImGui::SetCursorPos({ CalculateXOffsetForCenteringText("Awaken"), 15 });
                        ImGui::TextColored(ImVec4(ImColor(56, 101, 242)), "Awaken");
                        ImGui::PopFont();
                    }
                    else
                    {
                        ImVec2 childSize = ImGui::GetContentRegionAvail();

                        ImGui::SetCursorPos({ 0, 0 });
                        ImGui::Image((void*)g_pTexture, ImVec2(50, 50));
                    }
                   
                }
                ImGui::SetCursorPosY(65);

                ImVec2 windowSize = ImGui::GetWindowSize();
                ImVec2 imageSize(32, 32);
                ImVec2 imagePos((windowSize.x - imageSize.x) * 0.5f, (windowSize.y - imageSize.y - 16) * 0.5f);


                if (globals.tab == globals.login)
                {
                    if (!navbar_collapsed)
                    {


                        float spinnerWidth = 10.0f; // Set to the desired width of the spinner
                        float spinnerHeight = 10.0f * 3; // Assuming bars = 3

                        // Get the window size
                        float windowWidth = ImGui::GetWindowSize().x;
                        float windowHeight = ImGui::GetWindowSize().y;

                        // Calculate the horizontal and vertical positions to center the spinner
                        float posX = (windowWidth - spinnerWidth) * 0.5f;
                        float posY = (windowHeight - spinnerHeight) * 0.5f;

                        // Set the cursor position to center the spinner
                        ImGui::SetCursorPos(ImVec2(posX - 16.5, posY + 30));
                        ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(63, 92, 165), ImSpinner::Speed(6.8f), 3, true);
                    }
                }

                if (globals.tab == globals.init)
                {
                    if (!navbar_collapsed)
                    {


                        float spinnerWidth = 10.0f; // Set to the desired width of the spinner
                        float spinnerHeight = 10.0f * 3; // Assuming bars = 3

                        // Get the window size
                        float windowWidth = ImGui::GetWindowSize().x;
                        float windowHeight = ImGui::GetWindowSize().y;

                        // Calculate the horizontal and vertical positions to center the spinner
                        float posX = (windowWidth - spinnerWidth) * 0.5f;
                        float posY = (windowHeight - spinnerHeight) * 0.5f;

                        // Set the cursor position to center the spinner
                        ImGui::SetCursorPos(ImVec2(posX - 16.5, posY + 30));
                        ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(63, 92, 165), ImSpinner::Speed(6.8f), 3, true);
                    }
                }


                if (globals.tab == globals.dashboard)
                {
                    ImVec2 MAIN = ImGui::CalcTextSize("MAIN");

                    ImGui::PushFont(globals.PTRootUIBold); ImGui::SetCursorPosX(childSize.x / 2.0f - MAIN.x / 2.0f); ImGui::TextDisabled("MAIN");  ImGui::PopFont();

                    ImGui::Spacing();
                    ImGui::SetCursorPosX(10);
                    if (ImGuiCustom::tab2(ICON_FA_BARS, "Spoofer", subtabs == dashboard, !navbar_collapsed)) { subtabs = dashboard; }
                   
                    ImGui::SetCursorPosX(10);
                    if (ImGuiCustom::tab2(ICON_FA_CHALKBOARD_USER, "Cheats", subtabs == cheats, !navbar_collapsed)) { subtabs = cheats; }
          
                    //ImVec2 MISC = ImGui::CalcTextSize("OTHER");
                    //ImGui::PushFont(globals.PTRootUIBold); ImGui::SetCursorPosX(childSize.x / 2.0f - MISC.x / 2.0f); ImGui::TextDisabled("OTHER"); ImGui::PopFont();

                    ImGui::SetCursorPosX(10);
                    if (ImGuiCustom::tab2(ICON_FA_GEARS, "Utilities", subtabs == Utilities, !navbar_collapsed)) { subtabs = Utilities; }

                    ImGui::SetCursorPos({10, ImGui::GetWindowSize().y - 50});
                    if (ImGuiCustom::tab3(ICON_FA_ARROW_RIGHT, "Close", subtabs == Utilities, !navbar_collapsed)) { navbar_collapsed = !navbar_collapsed; }


                }
            }
            ImGui::EndChild();
        }

    }
    ImGui::End();

}



void ui::spoofing()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ImVec2 monitorSize = ImVec2(screenWidth, screenHeight);

    ImVec2 imageSize = ImVec2(128, 128); // change this to the size of your image
    ImVec2 spinnersize = ImVec2(44, 44); // change this to the size of your image
    ImVec2 windowPos = ImVec2((monitorSize.x - imageSize.x) / 2.0f, (monitorSize.y - imageSize.y - 250) / 2.0f);
    ImVec2 spinnerpos = ImVec2((monitorSize.x - spinnersize.x) / 2.0f, (monitorSize.y - spinnersize.y) / 2.0f);

    static float imagePosY = 0.0f;
    static float alpha = 0.0f;

    
    static int textIndex = 0;
    bool animationFinished = false;
    static std::vector<std::string> texts = {

"Communicating with the driver.",
"Connected with the driver.",
"---------------------------------",
"Spoofing Disks.",
"Starting next operation...",
"Spoofing SMBIOS.",
"Starting next operation...",
"Spoofing NIC.",
"Starting next operation...",
"Spoofing Boot Information.",
"Starting next operation...",
"Spoofing MAC.",
"Starting next operation...",
"Spoofing Registry.",
"Finished Spoofing.",
    };

    ImGui::SetNextWindowSize({ 10000, 10000 });

    ImGui::SetNextWindowPos({ -10000, 0 });
    
    ImGui::Begin("BLANK", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize);
    ImGui::End();


    ImGui::SetNextWindowSize(monitorSize);
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::Begin("Spoof", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize);
    {
        ImVec2 contentRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 imagePos = contentRegion / 2 - imageSize / 2;
        ImGui::SetCursorPos(imagePos + ImVec2(0, imagePosY));
        ImGui::Image(g_pTexture, imageSize);
        if (imagePosY > -90.f)
        {
            imagePosY -= ImGui::GetIO().DeltaTime * 50.0f; // change this line
            BlockInput(true);

        }


        if (imagePosY <= -90.f) {
            static float Alpha = 0;
            static bool Tick = false;
            static float Speed = 1.0f;

            if (!Tick || Alpha != 255)
            {
                Tick = false;
                if (!(Alpha >= 255))
                    Alpha += Speed;
                else if (Alpha >= 255)
                    Tick ^ 1;
            }


            ImGui::SetCursorPos(spinnerpos);
            ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(255, 255, 255), ImSpinner::Speed(6.8f), 3, true);

            ImGui::PushFont(globals.PTRootUIBold);
            const char* text = "Please wait while we spoof your PC"; // your text here
            ImVec2 textSize = ImGui::CalcTextSize(text);
            float currentY = ImGui::GetCursorPosY();

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - textSize.x) / 2.0f, currentY + 30.0f));
            ImGui::TextColored(ImVec4(ImColor(213, 211, 213, (int)Alpha)), "%s", text);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5, 5 });

            elapsedTime += ImGui::GetIO().DeltaTime;

            if (!animationFinished) {
                elapsedTime += ImGui::GetIO().DeltaTime;

                for (int i = 0; i < texts.size(); i++) {
                    if (elapsedTime >= i * 3.0f) {
                        ImVec2 textSize = ImGui::CalcTextSize(texts[i].c_str());
                        float lineHeight = textSize.y + 5.0f; // add some padding between lines
                        ImGui::SetCursorPosY(i * lineHeight);
                        ImGui::SetCursorPosX(5);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(166, 206, 168, (int)Alpha)));
                        ImGui::Text("%s", texts[i].c_str());
                        ImGui::PopStyleColor();
                    }
                }

                // reset the animation after all texts are shown
                if (elapsedTime >= texts.size() * 3.0f) {
                    animationFinished = true;
                    globals.tab = globals.dashboard;
                    BlockInput(false);

                    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
                    ::SetWindowRgn(hwnd, hRgn, TRUE);
                    ::UpdateWindow(hwnd);
                  
                    
                }
            }
            else {
                // show a message indicating that the animation has finished
                ImVec2 textSize = ImGui::CalcTextSize("Animation finished.");
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("Animation finished.");
                ImGui::PopStyleColor();
            }
            ImGui::PopStyleVar(1);

            // reset the animation after all texts are shown


            ImGui::PopFont();




        }

    }
    ImGui::End();
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;

    // colors
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    D3DXCreateTextureFromFileInMemory(
        dev,
        logo, // Pointer to your image data
        sizeof(logo),  // Size of the image data in bytes
        &g_pTexture);


    style.WindowBorderSize = 0.f;
    style.WindowPadding = { 0, 0 };
    style.FrameRounding = 5.f;
    style.ScrollbarSize = 5.f;
    style.ChildRounding = 5.f;
    style.FrameBorderSize = 3.f;

    style.Colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0);
    style.Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0);
    style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0);

    if (window_pos.x == 0) {
        RECT screen_rect{};
        GetWindowRect(GetDesktopWindow(), &screen_rect);
        screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
        window_pos = (screen_res - window_size) * 0.5f;

        // init images here
    }
}


