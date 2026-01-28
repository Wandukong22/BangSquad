// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Data/DataAsset/BSMapData.h"

const FMapInfo* UBSMapData::GetMapInfo(EStageIndex StageIndex, EStageSection Section) const
{
	for (const FMapInfo& Info : Maps)
	{
		if (Info.StageIndex == StageIndex && Info.Section == Section)
		{
			return &Info;
		}
	}
	return nullptr;
}

FString UBSMapData::GetMapPath(EStageIndex StageIndex, EStageSection Section) const
{
	const FMapInfo* Info = GetMapInfo(StageIndex, Section);
	if (Info && !Info->Level.IsNull())
	{
		return Info->Level.GetLongPackageName();
	}
	return FString();
}

FString UBSMapData::GetMapDisplayName(EStageIndex StageIndex, EStageSection Section) const
{
	const FMapInfo* Info = GetMapInfo(StageIndex, Section);
	return Info ? Info->DisplayName.ToString() : FString();
}
