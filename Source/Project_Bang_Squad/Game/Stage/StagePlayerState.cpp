// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Stage/StagePlayerState.h"

#include "Checkpoint.h"
#include "StageGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AStagePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStagePlayerState, Job);
	DOREPLIFETIME(AStagePlayerState, RespawnEndTime);
	DOREPLIFETIME(AStagePlayerState, MiniGameCheckpointIndex);
	DOREPLIFETIME(AStagePlayerState, StageCheckpointIndex);
	DOREPLIFETIME(AStagePlayerState, DeathCount);
	DOREPLIFETIME(AStagePlayerState, MiniGameRank);
}

void AStagePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AStagePlayerState* PS = Cast<AStagePlayerState>(PlayerState);
	if (PS)
	{
		PS->Job = Job;
		PS->RespawnEndTime = RespawnEndTime;
		PS->StageCheckpointIndex = StageCheckpointIndex;
		PS->DeathCount = DeathCount;

		//TODO: 체력 넣을거면 여기다가
	}
}

void AStagePlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

AStagePlayerState::AStagePlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStagePlayerState::SetRespawnEndTime(float NewTime)
{
	RespawnEndTime = NewTime;
	OnRep_RespawnEndTime();
}

void AStagePlayerState::SetJob(EJobType NewJob)
{
	Job = NewJob;
}

void AStagePlayerState::UpdateMiniGameCheckpoint(int32 NewIndex)
{
	if (NewIndex > MiniGameCheckpointIndex)
	{
		MiniGameCheckpointIndex = NewIndex;

		//TODO: 체크포인트 저장 UI는 여기서
	}
}

void AStagePlayerState::UpdateStageCheckpoint(int32 NewIndex)
{
	if (NewIndex > StageCheckpointIndex)
	{
		StageCheckpointIndex = NewIndex;

		//TODO: 체크포인트 저장 UI는 여기서
	}
}

void AStagePlayerState::SetMiniGameRank(int32 NewRank)
{
	MiniGameRank = NewRank;
}

float AStagePlayerState::GetMiniGameProgressScore() const
{
	if (MiniGameRank > 0)
	{
		return 1000000.f - (MiniGameRank * 10000.f);
	}
	
	//우선순위: 체크포인트 인덱스 > 다음 체크포인트까지의 거리
	//1: 체크포인트 인덱스
	float TotalScore = (float)MiniGameCheckpointIndex * 100.f;

	if (GetPawn())
	{
		float CurrentZ = GetPawn()->GetActorLocation().Z;
		TotalScore += (CurrentZ * 0.01f);
	}

	AStageGameState* GS = GetWorld()->GetGameState<AStageGameState>();
	if (!GS) return TotalScore;

	int32 NextIndex = MiniGameCheckpointIndex + 1;
	ACheckpoint** NextCP = GS->CheckpointMap.Find(NextIndex);

	if (NextCP && *NextCP && GetPawn())
	{
		//2: 다음 체크포인트까지의 거리
		//float Distance = FVector::Dist(GetPawn()->GetActorLocation(), (*NextCP)->GetActorLocation());
		//float DistanceScore = FMath::Clamp(1.f - (Distance / 5000.f), 0.f, 1.f);
		float Dist2D = FVector::Dist2D(GetPawn()->GetActorLocation(), (*NextCP)->GetActorLocation());
		float DistanceScore = FMath::Clamp(1.f - (Dist2D / 5000.f), 0.f, 0.99f);
		TotalScore += DistanceScore;
	}
	return TotalScore;
}

void AStagePlayerState::OnRep_RespawnEndTime()
{
	OnRespawnTimeChanged.Broadcast(RespawnEndTime);
}
