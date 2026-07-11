// Fill out your copyright notice in the Description page of Project Settings.


#include "WinArea.h"

#include "GameFramework/Character.h"

// Sets default values
AWinArea::AWinArea()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	WinAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WinAreaBox"));
	SetRootComponent(WinAreaBox);

	WinCondition = false;
}

// Called when the game starts or when spawned
void AWinArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWinArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (!WinCondition)
		{
			TArray<AActor*> OverlapActors;
			WinAreaBox->GetOverlappingActors(OverlapActors, ACharacter::StaticClass());

			WinCondition = (OverlapActors.Num() == 2);
			if (WinCondition)
			{
				// UE_LOG(LogTemp, Display, TEXT("Win!"));

				MulticastRPC_Win();
			}
		}
	}
}

void AWinArea::MulticastRPC_Win_Implementation()
{
	OnWinCondition.Broadcast();
}

