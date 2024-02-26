#pragma once

#include "Delegates/Delegate.h"

namespace JetBrains
{
    namespace EditorPlugin
    {
        class BlueprintReference;
    }
}

struct FAssetData;
class FMessageEndpoint;
class UBlueprint;

class RIDERBLUEPRINT_API BluePrintProvider {
public:

    static void AddAsset(FAssetData const& AssetData);

    static bool IsBlueprint(FString const& pathName);

    static void OpenBlueprint(JetBrains::EditorPlugin::BlueprintReference const& path, TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> const& messageEndpoint);
};
