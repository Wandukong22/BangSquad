#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformManager.generated.h"

class ABossPlatform;

UENUM(BlueprintType)
enum class EPlatformPattern : uint8
{
	CenterVertical, // 1. 중앙 세로 2줄
	TopBottom,      // 2. 상하 가로 2줄
	Corners2x2,     // 3. 좌상단+우하단 4칸씩
	Chessboard,     // 4. 체스판
	EdgeCenters     // 5. 각 모서리 중앙 2칸씩
};

UCLASS()
class PROJECT_BANG_SQUAD_API APlatformManager : public AActor
{
	GENERATED_BODY()

public:
	APlatformManager();
	virtual void BeginPlay() override;

	TArray<ABossPlatform*> GetPlatformsForMeteor();
	TArray<ABossPlatform*> GetPlatformsForPattern(EPlatformPattern Pattern);

	void StartMechanics();
	void ProcessPermanentDrop();
	void SpawnWindOrb();

protected:
	UPROPERTY(EditInstanceOnly, Category = "Config")
	TArray<TObjectPtr<ABossPlatform>> Platforms;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AActor> WindOrbClass;

	FTimerHandle Timer_PermDrop;
	FTimerHandle Timer_WindOrb;
};