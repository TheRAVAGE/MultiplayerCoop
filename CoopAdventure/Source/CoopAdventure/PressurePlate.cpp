// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

#include "TransporterComponent.h"
#include "Debug/Debug.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComponent);
	TriggerMesh->SetIsReplicated(true);
	
	auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));
	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.3f,3.3f,0.2f));
		TriggerMesh->SetRelativeLocation(FVector(0.f,0.0f,10.0f));
	}
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetIsReplicated(true);
	
	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4.f,4.f,0.5f));
		Mesh->SetRelativeLocation(FVector(0.f,0.0f,7.2f));
	}
	
	Transporter = CreateDefaultSubobject<UTransporterComponent>(TEXT("Transporter Component"));
	Transporter->MoveTime = 0.25f;
	Transporter->OwnerIsTriggerActor = true;


}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));
	
	FVector Point1 = GetActorLocation();
	FVector Point2 = Point1 + FVector(0.0f, 0.0f, -10.0f);
	Transporter->SetPoints(Point1, Point2);

}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		AActor* TriggerActor = 0;
		TriggerMesh->GetOverlappingActors(OverlappingActors);
		for (AActor* Actor : OverlappingActors)
		{
			if (Actor->ActorHasTag(FName("TriggerActor")))
			{
				TriggerActor = Actor;
				break;
			}
		}
		if (TriggerActor)
		{
			if (!Activated)
			{
				Activated = true;
				OnPressurePlateActivated.Broadcast();
				Debug::PrintMessage("Activated");
			}
		} else
		{
			if (Activated)
			{
				Activated = false;
				OnPressurePlateDeactivated.Broadcast();
				Debug::PrintMessage("Deactivated");
			}
		}
	}

}

