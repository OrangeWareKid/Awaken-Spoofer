#pragma once
#include "includes.h"
#include "imgui/imgui_tricks.hpp"

inline int tabs = 0;
inline int subtabs = 0;
inline int spoofertabs = 0;
inline float content_anim = 0.f;

namespace ImGuiCustom {
    bool Tab(const char* icon, const char* label, bool selected);
    void ImRotateStart();
    ImVec2 ImRotationCenter();
    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());



    bool begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
    bool begin_child_ex2(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
    bool begin_child(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    bool begin_child2(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    bool begin_child(ImGuiID id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    void end_child();

    bool tab2(const char* icon, const char* text, bool boolean, bool navbaractive);
    bool tab3(const char* icon, const char* text, bool boolean, bool navbaractive);
    bool subtab(const char* name, bool boolean);
    bool collapse_button(bool collapsed);
}

int rotation_start_index;
void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}




static float elapsedTime = 0.0f;

ImVec2 ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}

void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}
void Particles()
{
    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_radius[100];


    for (int i = 1; i < 100; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = 15.f;
            partile_speed[i] = 1 + rand() % 50;
            partile_radius[i] = rand() % 4;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 60));

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
        }

        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_radius[i], ImColor(255, 255, 255, 255 / 2));
    }

}

float CalculateXOffsetForCentering(float width)
{
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    return (contentRegionAvail.x - width) * 0.5f;
}

float CalculateXOffsetForCenteringText(const char* text)
{
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    return (contentRegionAvail.x - textSize.x) * 0.5f;
}

