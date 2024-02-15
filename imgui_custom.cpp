#include "imgui_custom.h"


using namespace ImGui;

#include <string>


void ImGuiCustom::ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImGuiCustom::ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}
void ImGuiCustom::ImRotateEnd(float rad, ImVec2 center)
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

bool ImGuiCustom::Tab(const char* icon, const char* label, bool selected) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiID id = window->GetID(label);

    ImVec2 p = window->DC.CursorPos;
    ImVec2 size({ window->Size.x, 24 });
    ImRect bb(p, p + size);

    ItemSize(bb);
    ItemAdd(bb, id);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    float anim = ImTricks::Animations::FastFloatLerp(std::string(label).append("tab.anim"), selected, 0.f, 1.f, 0.04f);
    auto col = ImTricks::Animations::FastColorLerp(GetColorU32(ImGuiCol_TextDisabled), GetColorU32(ImGuiCol_Text), anim);

    if (pressed)
        content_anim = 0.f;

    static float line_pos = 0.f;
    line_pos = ImLerp(line_pos, selected ? bb.Min.y - window->Pos.y : line_pos, 0.04f);
    window->DrawList->AddRectFilled({ bb.Max.x - 2, window->Pos.y + line_pos }, { bb.Max.x, window->Pos.y + line_pos + size.y }, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, anim), 2, ImDrawFlags_RoundCornersLeft);

    PushStyleColor(ImGuiCol_Text, col.Value);
    PushFont(GetIO().Fonts->Fonts[1]);
    RenderText({ bb.Min.x + 19, bb.GetCenter().y - CalcTextSize(icon).y / 2 }, icon);
    PopFont();
    RenderText({ bb.Min.x + 50, bb.GetCenter().y - CalcTextSize(label).y / 2 }, label);
    PopStyleColor();

    return pressed;
}

bool ImGuiCustom::begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;

    flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
    flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

    // Size
    const ImVec2 content_avail = GetContentRegionAvail();
    ImVec2 size = ImFloor(size_arg);
    const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
    if (size.x <= 0.0f)
        size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
    if (size.y <= 0.0f)
        size.y = ImMax(content_avail.y + size.y, 4.0f);

    SetNextWindowPos(ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y + 34.0f));
    SetNextWindowSize(size - ImVec2(0, 36));

    parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(22, 22, 22), 10.0f);
    parent_window->DrawList->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
    parent_window->DrawList->AddLine(parent_window->DC.CursorPos + ImVec2(1, 32), parent_window->DC.CursorPos + ImVec2(size.x - 1, 32), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
    parent_window->DrawList->AddRectFilledMultiColorRounded(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, 32), ImColor(22,  22, 22), ImColor(22,  22, 22), ImColor(22,  22, 22), ImColor(22,  22, 22), ImColor(22, 22, 22), 4.0f, ImDrawFlags_RoundCornersTopLeft);
    parent_window->DrawList->AddText(bold, 15.0f, parent_window->DC.CursorPos + ImVec2(16, 9), ImColor(125, 128, 133), name);

    ImVec2 bottomRectPos = ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y + size.y - 3);
    parent_window->DrawList->AddRectFilled(bottomRectPos, bottomRectPos + ImVec2(size.x - 1, 3), ImColor(56 / 255.0f, 101 / 255.0f, 242 / 255.0f), 3.0f, ImDrawFlags_RoundCornersBottom);


    // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
    const char* temp_window_name;
    if (name)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    const float backup_border_size = g.Style.ChildBorderSize;
    if (!border)
        g.Style.ChildBorderSize = 0.0f;
    bool ret = Begin(temp_window_name, NULL, flags);
    g.Style.ChildBorderSize = backup_border_size;

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;
    child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

    // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
    // While this is not really documented/defined, it seems that the expected thing to do.
    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    // Process navigation-in immediately so NavInit can run on first frame
    if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
        g.ActiveIdSource = ImGuiInputSource_Nav;
    }
    return ret;

}

bool ImGuiCustom::begin_child_ex2(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;

    flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
    flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

    // Size
    const ImVec2 content_avail = GetContentRegionAvail();
    ImVec2 size = ImFloor(size_arg);
    const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
    if (size.x <= 0.0f)
        size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too many issues)
    if (size.y <= 0.0f)
        size.y = ImMax(content_avail.y + size.y, 4.0f);

    SetNextWindowPos(ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y));
    SetNextWindowSize(size - ImVec2(0, 0));

    parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), ImColor(0, 0, 0), 4.0f);

    // Calculate the position of the small 3-pixel tall rectangle at the bottom
    ImVec2 bottomRectPos = ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y + size.y - 3);
    ImVec2 bottomRectSize = ImVec2(size.x - 1, 3);
    ImVec2 bottomRectMax = bottomRectPos + bottomRectSize;

    // Ensure there's no weird spacing by adjusting the cursor position
    parent_window->DC.CursorPos.y += size.y; // Adjusted to remove extra spacing

    // Build up name. If you need to append to a same child from multiple locations in the ID stack, use BeginChild(ImGuiID id) with a stable value.
    const char* temp_window_name;
    if (name)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    const float backup_border_size = g.Style.ChildBorderSize;
    if (!border)
        g.Style.ChildBorderSize = 0.0f;
    bool ret = Begin(temp_window_name, NULL, flags);
    g.Style.ChildBorderSize = backup_border_size;

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;
    child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

    // Process navigation-in immediately so NavInit can run on the first frame
    if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
        g.ActiveIdSource = ImGuiInputSource_Nav;
    }

    // Draw the small 3-pixel tall rectangle at the bottom using ImDrawList
    child_window->DrawList->AddRectFilled(bottomRectPos, bottomRectMax, ImColor(255 / 255.0f, 255 / 255.0f, 255 / 255.0f), 3.0f, ImDrawFlags_RoundCornersBottom);

    return ret;
}

bool ImGuiCustom::begin_child(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 11));
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    return begin_child_ex(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove);
}

bool ImGuiCustom::begin_child2(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 11));
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    return begin_child_ex2(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove);
}

bool ImGuiCustom::begin_child(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    IM_ASSERT(id != 0);
    return begin_child_ex(NULL, id, size_arg, border, extra_flags);
}

void ImGuiCustom::end_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   

    g.WithinEndChild = true;
    if (window->BeginCount > 1)
    {
        End();
    }
    else
    {
        ImVec2 sz = window->Size;
        if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) 
            sz.x = ImMax(4.0f, sz.x);
        if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
            sz.y = ImMax(4.0f, sz.y);
        End();

        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
        ItemSize(sz);
        if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavWindowHasScrollY) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ItemAdd(bb, window->ChildId);
            RenderNavHighlight(bb, window->ChildId);

            // When browsing a window that has no activable items (scroll only) we keep a highlight on the child (pass g.NavId to trick into always displaying)
            if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
        }
        else
        {
            // Not navigable into
            ItemAdd(bb, 0);
        }
        if (g.HoveredWindow == window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    ImGui::PopStyleVar(2);
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX; // To enforce a carriage return
}

struct tab_element {
    float element_opacity, rect_opacity, text_opacity;
};

bool ImGuiCustom::tab2(const char* name, const char* text, bool boolean, bool navbaractive)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);
    ImVec2 label_size = ImGui::CalcTextSize(name, NULL, false, -1.0f);
    ImVec2 label_size1 = ImGui::CalcTextSize(text, NULL, false, -1.0f);

    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + 135, pos.y + 31));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y + 5), style.FramePadding.y);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <ImGuiID, tab_element> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end()) {
        anim.insert({ id, { 0.0f, 0.0f, 0.0f } });
        it_anim = anim.find(id);
    }

    it_anim->second.rect_opacity = ImLerp(it_anim->second.rect_opacity, (boolean ? 1.0f : 0.0f), 0.15f * (1.0f - ImGui::GetIO().DeltaTime));

    // Calculate the width of the horizontal rectangle (set it to the full width)
    float rectWidth = (rect.Max.x - rect.Min.x);

    // Calculate the position of the horizontal rectangle at the bottom
    ImVec2 rectPos(rect.Min.x, rect.Max.y - 4);

    // Draw the horizontal rectangle
   /* window->DrawList->AddRectFilled(rectPos, ImVec2(rectPos.x + rectWidth, rect.Max.y), ImColor(113 / 255.0f, 141 / 255.0f, 178 / 255.0f, it_anim->second.rect_opacity), 7.0f, ImDrawFlags_RoundCornersBottom);*/

    if (navbaractive)
    {
        it_anim->second.element_opacity = ImLerp(it_anim->second.element_opacity, (boolean ? 1.f : hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
        it_anim->second.text_opacity = ImLerp(it_anim->second.text_opacity, (boolean ? 1.0f : hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));

        window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f, it_anim->second.element_opacity), 6.0f);


        window->DrawList->AddText(medium, 15.0f, ImVec2(40, rect.Min.y + rect.Max.y - label_size.y + 3) / 2, ImColor(1.0f, 1.0f, 1.0f, it_anim->second.text_opacity), name);
        window->DrawList->AddText(medium, 15.0f, (rect.Min + rect.Max - label_size1) / 2, ImColor(1.0f, 1.0f, 1.0f, it_anim->second.text_opacity), text);
    }
    if (!navbaractive)
    {
        it_anim->second.element_opacity = ImLerp(it_anim->second.element_opacity, (boolean ? 1.f : hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
        it_anim->second.text_opacity = ImLerp(it_anim->second.text_opacity, (boolean ? 1.0f : hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));

        window->DrawList->AddRectFilled(rect.Min, ImVec2(rect.Min.x + 32, rect.Min.y + 30), ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f, it_anim->second.element_opacity), 6.0f);

        //window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f, it_anim->second.element_opacity), 6.0f);

        // Show only the name text and shrink it down
        window->DrawList->AddText(medium, 15.0f, ImVec2(40, rect.Min.y + rect.Max.y - label_size.y + 3) / 2, ImColor(1.0f, 1.0f, 1.0f, it_anim->second.text_opacity), name);

    }



    return pressed;
}


struct subtab_element {
    float element_opacity, rect_opacity, text_opacity;
};

bool ImGuiCustom::subtab(const char* name, bool boolean)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);
    ImVec2 label_size = ImGui::CalcTextSize(name, NULL, false, -1.0f);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + 100, pos.y + 31));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y + 5), style.FramePadding.y);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <ImGuiID, tab_element> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end()) {
        anim.insert({ id, { 0.0f, 0.0f, 0.0f } });
        it_anim = anim.find(id);
    }

    it_anim->second.rect_opacity = ImLerp(it_anim->second.rect_opacity, (boolean ? 1.0f : 0.0f), 0.15f * (1.0f - ImGui::GetIO().DeltaTime));

    // Calculate the width of the horizontal rectangle (set it to the full width)
    float rectWidth = (rect.Max.x - rect.Min.x);

    // Calculate the position of the horizontal rectangle at the bottom
    ImVec2 rectPos(rect.Min.x, rect.Max.y - 4);

    // Draw the horizontal rectangle
    window->DrawList->AddRectFilled(rectPos, ImVec2(rectPos.x + rectWidth, rect.Max.y), ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f, it_anim->second.rect_opacity), 7.0f, ImDrawFlags_RoundCornersBottom);

    it_anim->second.element_opacity = ImLerp(it_anim->second.element_opacity, (boolean ? 0.04f : hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
    it_anim->second.text_opacity = ImLerp(it_anim->second.text_opacity, (boolean ? 1.0f : hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));

    window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(1.0f, 1.0f, 1.0f, it_anim->second.element_opacity), 3.0f);
    window->DrawList->AddText((rect.Min + rect.Max - label_size) / 2, ImColor(1.0f, 1.0f, 1.0f, it_anim->second.text_opacity), name);
    /*window->DrawList->AddText(medium, 15.0f, (rect.Min + rect.Max - label_size1) / 2, ImColor(1.0f, 1.0f, 1.0f, it_anim->second.text_opacity), text);*/




    return pressed;
}


bool ImGuiCustom::collapse_button(bool collapsed) {
    PushStyleVar(ImGuiStyleVar_Alpha, 255);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(15, 17, 16, 0)));
    BeginChild("123", { 17, 17 });

    ImGuiWindow* window = GetCurrentWindow();



    ImGuiID id = window->GetID("collapse_button");



    ImVec2 p = window->DC.CursorPos;
    ImVec2 size(GetWindowSize());
    ImRect bb(p, p + size);

    ItemSize(bb);
    ItemAdd(bb, id);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    window->DrawList->AddCircleFilled(bb.GetCenter(), bb.GetWidth() / 2, ImColor(23, 25, 24));
    window->DrawList->AddCircle(bb.GetCenter(), bb.GetWidth() / 2, GetColorU32(ImGuiCol_Border));

    float anim = ImTricks::Animations::FastFloatLerp("collapse_button.anim", collapsed, 0.f, 1.f, 0.04f);

    ImRotateStart();
    RenderArrow(window->DrawList, bb.GetCenter() - ImVec2(2 + 2 * !anim, 6), ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f), ImGuiDir_Down, 9.f);
    ImRotateEnd(3.14f * anim);

    EndChild();

    PopStyleVar(1);
    PopStyleColor(1);
    return pressed;
}


struct tab_element1 {
    float element_opacity, rect_opacity, text_opacity;
};

bool ImGuiCustom::tab3(const char* name, const char* text, bool boolean, bool navbaractive)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);
    ImVec2 label_size = ImGui::CalcTextSize(name, NULL, false, -1.0f);
    ImVec2 label_size1 = ImGui::CalcTextSize(text, NULL, false, -1.0f);

    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + 135, pos.y + 31));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y + 5), style.FramePadding.y);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <ImGuiID, tab_element> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end()) {
        anim.insert({ id, { 0.0f, 0.0f, 0.0f } });
        it_anim = anim.find(id);
    }

    it_anim->second.rect_opacity = ImLerp(it_anim->second.rect_opacity, (boolean ? 1.0f : 0.0f), 0.15f * (1.0f - ImGui::GetIO().DeltaTime));

    // Calculate the width of the horizontal rectangle (set it to the full width)
    float rectWidth = (rect.Max.x - rect.Min.x);

    // Calculate the position of the horizontal rectangle at the bottom
    ImVec2 rectPos(rect.Min.x, rect.Max.y - 4);

    // Draw the horizontal rectangle
   /* window->DrawList->AddRectFilled(rectPos, ImVec2(rectPos.x + rectWidth, rect.Max.y), ImColor(113 / 255.0f, 141 / 255.0f, 178 / 255.0f, it_anim->second.rect_opacity), 7.0f, ImDrawFlags_RoundCornersBottom);*/

    if (navbaractive)
    {
        it_anim->second.element_opacity = ImLerp(it_anim->second.element_opacity, (boolean ? 1.f : hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
        it_anim->second.text_opacity = ImLerp(it_anim->second.text_opacity, (boolean ? 1.0f : hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));

        window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f), 6.0f);


        window->DrawList->AddText(medium, 15.0f, ImVec2(40, rect.Min.y + rect.Max.y - label_size.y + 3) / 2, ImColor(1.0f, 1.0f, 1.0f), ICON_FA_ARROW_LEFT);
        window->DrawList->AddText(medium, 15.0f, (rect.Min + rect.Max - label_size1) / 2, ImColor(1.0f, 1.0f, 1.0f), text);
    }
    if (!navbaractive)
    {
        it_anim->second.element_opacity = ImLerp(it_anim->second.element_opacity, (boolean ? 1.f : hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
        it_anim->second.text_opacity = ImLerp(it_anim->second.text_opacity, (boolean ? 1.0f : hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));

        window->DrawList->AddRectFilled(rect.Min, ImVec2(rect.Min.x + 32, rect.Min.y + 30), ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f), 6.0f);

        //window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(54 / 255.f, 109 / 255.f, 242 / 255.f, it_anim->second.element_opacity), 6.0f);

        // Show only the name text and shrink it down
        window->DrawList->AddText(medium, 15.0f, ImVec2(40, rect.Min.y + rect.Max.y - label_size.y + 3) / 2, ImColor(1.0f, 1.0f, 1.0f), ICON_FA_ARROW_RIGHT);

    }



    return pressed;
}