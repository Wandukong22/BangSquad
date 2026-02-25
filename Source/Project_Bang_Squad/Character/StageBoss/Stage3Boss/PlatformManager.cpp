#include "PlatformManager.h"
#include "BossPlatform.h" 

APlatformManager::APlatformManager() {}

void APlatformManager::BeginPlay()
{
	Super::BeginPlay();
}

void APlatformManager::StartMechanics()
{
	GetWorldTimerManager().SetTimer(Timer_PermDrop, this, &APlatformManager::ProcessPermanentDrop, 30.0f, true);
	GetWorldTimerManager().SetTimer(Timer_WindOrb, this, &APlatformManager::SpawnWindOrb, 15.0f, true);
}

void APlatformManager::ProcessPermanentDrop()
{
	TArray<ABossPlatform*> Candidates;

	for (const auto& P : Platforms)
	{
		if (P && P->IsWalkable())
		{
			Candidates.Add(P.Get());
		}
	}

	if (Candidates.Num() > 0)
	{
		int32 Idx = FMath::RandRange(0, Candidates.Num() - 1);
		Candidates[Idx]->SetWarning(true);

		FTimerHandle DropHandle;
		ABossPlatform* TargetPlatform = Candidates[Idx];
		GetWorldTimerManager().SetTimer(DropHandle, [TargetPlatform]() {
			if (TargetPlatform) TargetPlatform->MoveDown(true);
			}, 1.0f, false);
	}
}

void APlatformManager::SpawnWindOrb()
{
	if (!WindOrbClass) return;

	TArray<ABossPlatform*> Candidates;
	for (const auto& P : Platforms)
	{
		if (P && P->IsWalkable())
		{
			Candidates.Add(P.Get());
		}
	}

	if (Candidates.Num() > 0)
	{
		int32 Idx = FMath::RandRange(0, Candidates.Num() - 1);
		FVector SpawnLoc = Candidates[Idx]->GetActorLocation() + FVector(0, 0, 100);
		GetWorld()->SpawnActor<AActor>(WindOrbClass, SpawnLoc, FRotator::ZeroRotator);
	}
}

TArray<ABossPlatform*> APlatformManager::GetPlatformsForMeteor()
{
	TArray<ABossPlatform*> Result;
	TArray<ABossPlatform*> Occupied;
	TArray<ABossPlatform*> Others;

	for (const auto& P : Platforms)
	{
		if (!P || !P->IsWalkable()) continue;

		ABossPlatform* RawP = P.Get();

		if (RawP->GetPlayerCount() > 0)
			Occupied.Add(RawP);
		else
			Others.Add(RawP);
	}

	int32 TakeFromOccupied = (Occupied.Num() >= 2) ? 2 : (Occupied.Num() > 0 ? 1 : 0);

	if (Occupied.Num() > 0)
	{
		for (int32 i = 0; i < Occupied.Num(); ++i)
			Occupied.Swap(i, FMath::RandRange(0, Occupied.Num() - 1));

		for (int32 i = 0; i < TakeFromOccupied; ++i)
			Result.Add(Occupied[i]);
	}

	int32 Needed = 5 - Result.Num();
	if (Others.Num() > 0)
	{
		for (int32 i = 0; i < Others.Num(); ++i)
			Others.Swap(i, FMath::RandRange(0, Others.Num() - 1));

		for (int32 i = 0; i < Needed && i < Others.Num(); ++i)
			Result.Add(Others[i]);
	}

	return Result;
}

TArray<ABossPlatform*> APlatformManager::GetPlatformsForPattern(EPlatformPattern Pattern)
{
	TArray<ABossPlatform*> Result;
	TArray<int32> TargetIndices;

	// 4x4 Grid Index Map:
	//  0  1  2  3
	//  4  5  6  7
	//  8  9 10 11
	// 12 13 14 15

	switch (Pattern)
	{
	case EPlatformPattern::CenterVertical: // 1. 중앙 세로 2줄 (1,2 열)
		TargetIndices = { 1, 5, 9, 13,  2, 6, 10, 14 };
		break;

	case EPlatformPattern::TopBottom: // 2. 상하 가로 2줄 (0, 3 행)
		TargetIndices = { 0, 1, 2, 3,  12, 13, 14, 15 };
		break;

	case EPlatformPattern::Corners2x2: // 3. 좌상단 2x2 + 우하단 2x2
		// 좌상단: 0,1,4,5 / 우하단: 10,11,14,15
		TargetIndices = { 0, 1, 4, 5,  10, 11, 14, 15 };
		break;

	case EPlatformPattern::Chessboard: // 4. 체스판 (XOXO)
		// Row 0(XOXO): 0, 2
		// Row 1(OXOX): 5, 7
		// Row 2(XOXO): 8, 10
		// Row 3(OXOX): 13, 15
		TargetIndices = { 0, 2, 5, 7, 8, 10, 13, 15 };
		break;

	case EPlatformPattern::EdgeCenters: // 5. 각 모서리 변의 중앙 2칸 (총 8칸)
		// Top(1,2), Bottom(13,14), Left(4,8), Right(7,11)
		TargetIndices = { 1, 2, 13, 14, 4, 8, 7, 11 };
		break;
	}

	// 인덱스를 실제 발판으로 변환 (유효성 및 영구하강 체크)
	for (int32 Idx : TargetIndices)
	{
		if (Platforms.IsValidIndex(Idx) && Platforms[Idx])
		{
			// [중요] IsWalkable() 내부에서 영구하강(!bIsPermanentlyDown) 여부를 체크함
			if (Platforms[Idx]->IsWalkable())
			{
				Result.Add(Platforms[Idx].Get());
			}
		}
	}

	return Result;
}