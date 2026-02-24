// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.h"

#include "MiniGameState.h"
#include "Net/UnrealNetwork.h"
#include "Project_Bang_Squad/Game/Stage/Checkpoint.h"

AMiniGamePlayerState::AMiniGamePlayerState()
{
}

void AMiniGamePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMiniGamePlayerState, MiniGameRank);
	DOREPLIFETIME(AMiniGamePlayerState, MiniGameCheckpointIndex);
}

void AMiniGamePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AMiniGamePlayerState* PS = Cast<AMiniGamePlayerState>(PlayerState);
	if (PS)
	{
		PS->MiniGameRank = MiniGameRank;
	}
}

void AMiniGamePlayerState::SetMiniGameRank(int32 NewRank)
{
	MiniGameRank = NewRank;
}

void AMiniGamePlayerState::UpdateMiniGameCheckpoint(int32 NewIndex)
{
	if (NewIndex > MiniGameCheckpointIndex)
	{
		MiniGameCheckpointIndex = NewIndex;
	}
}

float AMiniGamePlayerState::GetMiniGameProgressScore() const
{
	if (MiniGameRank > 0)
	{
		return 1000000.f - (MiniGameRank * 10000.f);
	}

	float TotalScore = static_cast<float>(MiniGameCheckpointIndex) * 100.f;

	if (GetPawn())
	{
		//float CurrentZ = GetPawn()->GetActorLocation().Z;
		//TotalScore += (CurrentZ * 0.01f);
	}

	AInGameState* GS = GetWorld()->GetGameState<AInGameState>();
	if (!GS) return TotalScore;

	int32 NextIndex = MiniGameCheckpointIndex + 1;
	ACheckpoint** NextCP = GS->CheckpointMap.Find(NextIndex);

	if (NextCP && *NextCP && GetPawn())
	{
		//float Dist2D = FVector::Dist2D(GetPawn()->GetActorLocation(), (*NextCP)->GetActorLocation());
		float Dist = FVector::Dist(GetPawn()->GetActorLocation(), (*NextCP)->GetActorLocation());
		float DistanceScore = FMath::Clamp(1.f - (Dist / 5000.f), 0.f, 0.99f);
		TotalScore += DistanceScore;
	}
	return TotalScore;
}
