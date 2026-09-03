#pragma once

#include "CoreMinimal.h"
#include "LobbyGameTypes.generated.h"

UENUM()
enum class EJobClaimResult : uint8
{
	Unknown,
	Success,
	InvalidPlayer,
	InvalidPhase,
	InvalidJob,
	AlreadyTaken,
	InternalError,
};
