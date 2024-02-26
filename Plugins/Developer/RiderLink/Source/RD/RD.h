// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(FLogRDModule, Log, All);

class FRDModule : public IModuleInterface
{
public:
	FRDModule() = default;
	~FRDModule() = default;
};
