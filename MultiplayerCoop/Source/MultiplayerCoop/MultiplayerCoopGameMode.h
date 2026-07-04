// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerCoopGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerCoopGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerCoopGameMode();
	
	UFUNCTION(BlueprintCallable)
	void HostGame();
	
	UFUNCTION(BlueprintCallable)
	void JoinGame();
};



