// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGameGoal.h"

#include "MiniGameMode.h"

AMiniGameGoal::AMiniGameGoal()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	bReplicates = true;
}

void AMiniGameGoal::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!HasAuthority()) return;
	if (!InstigatorPawn) return;

	//게임 모드 가져와서 보고
	AMiniGameMode* GM = Cast<AMiniGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->OnPlayerReachedGoal(InstigatorPawn->GetController());
	}
	
}

