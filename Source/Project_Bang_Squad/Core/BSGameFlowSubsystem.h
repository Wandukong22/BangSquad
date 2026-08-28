// 

#pragma once

#include "CoreMinimal.h"
#include "BSGameTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSGameFlowSubsystem.generated.h"

class UBSSessionSubsystem;
class UBSMapData;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UBSGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	TObjectPtr<UBSSessionSubsystem> SessionSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "BS|Data")
	TObjectPtr<UBSMapData> MapData;
	bool bIsTraveling = false;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void ServerTravelToStage(EStageIndex StageIndex, EStageSection Section);
	void HostTravelToLobby();
	void ClientTravelToAddress(const FString& Address);
	void ClientTravelToMainMenu();

private:
	void HandleCreateSessionSucceeded();
	void HandleJoinSessionSucceeded(const FString& Address);
	void HandleLeaveSessionSucceeded();
	void HandlePostLoadMap(UWorld* LoadedWorld);
	void HandleNetworkFailure(
		UWorld* World,
		UNetDriver* NetDriver,
		ENetworkFailure::Type FailureType,
		const FString& ErrorString);
	void HandleTravelFailure(
		UWorld* World,
		ETravelFailure::Type FailureType,
		const FString& ErrorString);
};
