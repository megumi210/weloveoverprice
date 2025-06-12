#pragma once
#include <Windows.h>

HWND Entryhwnd = NULL;
int processid = 0;

namespace offsets // updated with new values from first block
{
    DWORD
      
        FnameState = 0xB4F31C0,
        fnamekey = 0xB4820B8,
        FnamePool = 0xB4C0200,
        persistent_level = 0x38,//done
        local_player_array = 0x40,
        fnamepool = 0xB44F0C0,
        camera_fov = 0x11f8,
        local_player_pawn = 0x40,
        Gameinstance = 0x1a0, //done
        Ulevel = 0x38,//done
        LocalPlayers = 0x40,//done
        PlayerController = 0x38,//done
        PlayerCameraManager = 0x460,//done
        MyHUD = 0x458,//maybe
        AcknowledgedPawn = 0x448,//done
        PlayerState = 0x3d8,//done
        TeamComponent = 0x610,//done
        TeamID = 0xf8,//done
        UniqueID = 0x38,//done
        FNameID = 0x18,//done
        AActorArray = 0xA0,//done
        RootComponent = 0x238,//done
        RelativeLocation = 0x164,//done
        MeshComponent = 0x418,//done
        DamageHandler = 0xA08,//done
        bIsDormant = 0x100,//done
        Health = 0x1E0, //done
        ComponentToWorld = 0x250,//done
        BoneArray = 0x5c8,//done
        bone_array = 0x5D8,
        BoneArrayCache = 0x5F8,//done
        BoneCount = 0x5D0,//done
        GWorld = 0x9FA0060, //done
        is_visible = 0x501,
        portrait_map = 0x11c8,
        component_to_world = 0x250,
        mesh = 0x418,
        last_submit_time = 0x380,
        last_render_time = 0x384,
        SpikeTimer = 0x4cc,
        CurrentDefuseSection = 0x4fc,
        ControlRotation = 0x428; //done

}


namespace Settings
{
     bool bHVCI = true;
     bool bMenu = true;

    namespace Visuals
    {
         bool bSnaplines = false;
         bool bDistance = false;
         bool bBox = false;
         bool bBoxOutlined = true;
         bool bHealth = true;

         float BoxWidth = 1.0f;
    }
}