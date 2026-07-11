// Fill out your copyright notice in the Description page of Project Settings.


#include "TransporterComponent.h"

#include "CollectableKey.h"
#include "PressurePlate.h"
#include "Debug/Debug.h"

UTransporterComponent::UTransporterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
 
 	SetIsReplicatedByDefault(true);
 
 	MoveTime = 3.0f;
 	ActivatedTriggerCount = 0;
 
 	ArePointsSet = false;
 	StartPoint = FVector::Zero();
 	EndPoint = FVector::Zero();

}


// Called when the game starts
void UTransporterComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (OwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	for (AActor* TA : TriggerActors)
	{
		APressurePlate *PressurePlateActor = Cast<APressurePlate>(TA);
		if (PressurePlateActor)
		{
			PressurePlateActor->OnPressurePlateActivated.AddDynamic(this, &UTransporterComponent::OnTriggerActorActivated);
			PressurePlateActor->OnPressurePlateDeactivated.AddDynamic(this, &UTransporterComponent::OnTriggerActorDeactivated);
			// Debug::PrintMessage(FString::Printf(TEXT("Added OnPressurePlateActivated delegate for %s"), *PressurePlateActor->GetName()));
			continue;
		}

		ACollectableKey* KeyActor = Cast<ACollectableKey>(TA);
		if (KeyActor)
		{
			KeyActor->OnCollected.AddDynamic(this, &UTransporterComponent::OnTriggerActorActivated);
			// Debug::PrintMessage(FString::Printf(TEXT("Added OnCollected delegate for %s"), *KeyActor->GetName()));
			continue;
		}


	}
	
}


void UTransporterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TriggerActors.Num() > 0)
	{
		AllTriggerActorsTriggered = (ActivatedTriggerCount >= TriggerActors.Num());
		// if (AllTriggerActorsTriggered)
		// {
		// 	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString("AllTriggerActorsTriggered!"));
		// }
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner && MyOwner->HasAuthority() && ArePointsSet)
	{
		FVector CurrentLocation = MyOwner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime;

		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;
		if (!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
			MyOwner->SetActorLocation(NewLocation);
		}
	}
}

void UTransporterComponent::SetPoints(FVector NewStartPoint, FVector NewEndPoint)
{
	if (NewStartPoint.Equals(NewEndPoint)) return;
    
    	StartPoint = NewStartPoint;
    	EndPoint = NewEndPoint;
    	ArePointsSet = true;

}

void UTransporterComponent::OnTriggerActorActivated()
{
	ActivatedTriggerCount++;

	// FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, Msg);

}

void UTransporterComponent::OnTriggerActorDeactivated()
{
	ActivatedTriggerCount--;
}

