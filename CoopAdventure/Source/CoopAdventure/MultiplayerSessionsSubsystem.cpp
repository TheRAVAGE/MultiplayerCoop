// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSubsystem.h"

#include "Debug/Debug.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	// Debug::PrintMessage(TEXT("MSS Contructed"), FColor::Green, true);
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Debug::PrintString(GetWorld(), NAME_None, TEXT("MSS Initialized"), 10.f, FColor::Green, true);
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		Debug::PrintMessage(FString::Printf(TEXT("Found Subsystem : %s"), *Subsystem->GetSubsystemName().ToString()));
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			Debug::PrintMessage(TEXT("Found Session Interface"));
		}
		else
		{
			Debug::PrintMessage(TEXT("Session Interface Not Found"));
		}
	}
	
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	// Debug::LogMsg(TEXT("MSS Deinitialized"));
}
