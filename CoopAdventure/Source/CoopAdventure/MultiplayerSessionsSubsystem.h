// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoinSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class COOPADVENTURE_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionsSubsystem();
	
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;
	
	IOnlineSessionPtr SessionInterface;
	
	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName);
	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName);
	
	void CreateSessionComplete(FName SessionName, bool bWasSucccessful);
	void DestroySessionComplete(FName SessionName, bool bWasSucccessful);
	void FindSessionComplete(bool bWasSucccessful);
	void JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	bool bIsCreatingServerAfterDestroy;
	FString DestroyServerName;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FString ServerNameToFind;
	FName CurrentSessionName;
	
	UPROPERTY(BlueprintAssignable)
	FCreateSessionComplete OnCreateSessionComplete;
	UPROPERTY(BlueprintAssignable)
	FJoinSessionComplete OnJoinSessionComplete;
	
	UPROPERTY(BlueprintReadWrite)
	FString GameMapPath;
};

