// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

#include "Debug/Debug.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	// Debug::PrintMessage(TEXT("MSS Contructed"), FColor::Green, true);
	
	bIsCreatingServerAfterDestroy = false;
	DestroyServerName = FString();
	ServerNameToFind = FString();
	CurrentSessionName = FName("Coop Adventure Session Name");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Debug::PrintString(GetWorld(), NAME_None, TEXT("MSS Initialized"), 10.f, FColor::Green, true);
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		Debug::PrintMessage(FString::Printf(TEXT("Found Subsystem : %s"), *Subsystem->GetSubsystemName().ToString()));
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			Debug::PrintMessage(TEXT("Found Session Interface"));
			
			//Binding Functions to Delegates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::CreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::DestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::FindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::JoinSessionComplete);
			
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

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
	Debug::PrintMessage(FString::Printf(TEXT("Creating Server : %s"), *ServerName), FColor::Green, true);
	
	if (ServerName.IsEmpty())
	{
		Debug::PrintMessage(FString::Printf(TEXT("Server Name is Empty")));
		OnCreateSessionComplete.Broadcast(false);
		return;
	}
	
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(CurrentSessionName);
	if (ExistingSession)
	{
		Debug::PrintMessage(FString::Printf(TEXT("Existing Session Found : %s, Destroying that session"), *CurrentSessionName.ToString()));
		bIsCreatingServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(CurrentSessionName);
		return;

	}
	
	FOnlineSessionSettings SessionSettings;
	
	//Session Settings
	SessionSettings.bAllowJoinInProgress = true; // Allows joining a match that started
	SessionSettings.bIsDedicated = false; // Is this a dedicated server
	SessionSettings.bShouldAdvertise = true; // Should this session be available for anybody to join
	SessionSettings.NumPublicConnections = 2; // Max Number of Players
	SessionSettings.bUseLobbiesIfAvailable = true; // Use Lobbies system if available
	SessionSettings.bUsesPresence = true; // Use Presence (region based matchmaking) system if available
	SessionSettings.bAllowJoinViaPresence = true; // Allow joining via presence (region based matchmaking) system if available
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName()=="NULL")?true:false; 
	// Sets LAN match if its Null and if its steam it's not a LAN Match
	
	SessionSettings.Set(FName("SERVER_NAME"), ServerName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, CurrentSessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
	Debug::PrintMessage(FString::Printf(TEXT("Finding Server : %s"), *ServerName), FColor::Yellow, true);
	
	if (ServerName.IsEmpty())
	{
		Debug::PrintMessage(FString::Printf(TEXT("Server Name is Empty")));
		OnJoinSessionComplete.Broadcast(false);
		return;
	}
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName()=="NULL")?true:false; // Sets LAN match if its Null and if its steam it's not a LAN Match
	SessionSearch->MaxSearchResults = 9999; // Max number of search results
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // Search for sessions with presence enabled
	
	ServerNameToFind = ServerName;
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::CreateSessionComplete(FName SessionName, bool bWasSucccessful)
{
	Debug::PrintMessage(FString::Printf(TEXT("Create Session Complete : %s, Success : %s"), *SessionName.ToString(), bWasSucccessful ? TEXT("True") : TEXT("False")), FColor::Green, true);
	OnCreateSessionComplete.Broadcast(bWasSucccessful);
	if (bWasSucccessful)
	{
		FString Path = "/Game/ThirdPerson/Maps/ThirdPersonMap?listen";
		if (!GameMapPath.IsEmpty())
		{
			Path = FString::Printf(TEXT("%s?listen"), *GameMapPath);
		}
		GetWorld()->ServerTravel(Path);
	}
}

void UMultiplayerSessionsSubsystem::DestroySessionComplete(FName SessionName, bool bWasSucccessful)
{
	// Debug::PrintMessage(FString::Printf(TEXT("Destroy Session Complete : %s, Success : %s"), *SessionName.ToString(), bWasSucccessful ? TEXT("True") : TEXT("False")), FColor::Green, true);
	
	if (bIsCreatingServerAfterDestroy)
	{
		bIsCreatingServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionsSubsystem::FindSessionComplete(bool bWasSucccessful)
{
	if (!bWasSucccessful) return;
	if (ServerNameToFind.IsEmpty()) return;
	
	OnJoinSessionComplete.Broadcast(bWasSucccessful);
	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	FOnlineSessionSearchResult* FoundSession = 0;
	Debug::PrintMessage(FString::Printf(TEXT("Find Sessions Complete, Number of Sessions found : %d"), Results.Num()), FColor::Green, true);
	if (bWasSucccessful)
	{
		for (FOnlineSessionSearchResult Result: Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "NAME_None";
				Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);
				Debug::PrintMessage(FString::Printf(TEXT("Found Session by : %s, Server Name : %s"), *Result.Session.OwningUserName, *ServerName), FColor::Green, true);
				
				if (ServerName.Equals(ServerNameToFind))
				{
					FoundSession = &Result;
					Debug::PrintMessage(FString::Printf(TEXT("Found Session by : %s, Server Name : %s"), *Result.Session.OwningUserName, *ServerName), FColor::Green, true);
					break;
				}
			}
		}
		if (FoundSession)
		{
			SessionInterface->JoinSession(0, CurrentSessionName, *FoundSession);
		} else
		{
			OnJoinSessionComplete.Broadcast(false);
			Debug::PrintMessage("Couldn't find server");
			ServerNameToFind = FString();
		}
	}
}

void UMultiplayerSessionsSubsystem::JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	OnJoinSessionComplete.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		Debug::PrintMessage(FString::Printf(TEXT("Join Session Complete : %s, Success : %s"), *SessionName.ToString(), LexToString(Result)), FColor::Green, true);
		
		FString Address = FString();
		if (SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			Debug::PrintMessage(FString::Printf(TEXT("Joining Session at : %s"), *Address), FColor::MakeRandomColor(), true);
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				Debug::PrintMessage(FString::Printf(TEXT("Player Controller found, traveling to : %s"), *Address), FColor::Green, true);
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}else
			{
				Debug::PrintMessage(FString::Printf(TEXT("Player Controller not found")), FColor::Red, true);
			}
		} else
		{
			OnJoinSessionComplete.Broadcast(false);
			Debug::PrintMessage(FString::Printf(TEXT("Failed to get resolved connect string")), FColor::Red, true);
		}
	}
}
