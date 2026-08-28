// 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BSGameSettings.generated.h"

class UBSMapData;
/**
 *
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName = "Bang Squad"))
class PROJECT_BANG_SQUAD_API UBSGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Game Flow")
	TSoftObjectPtr<UBSMapData> MapData;
};
