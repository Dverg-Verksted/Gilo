// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameConfigModel/ParameterLists/ParameterList.h"
#include "AudioParametersList.generated.h"

#define GET_LOCAL_GAME_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)                                                                                                                                  \
	MakeShared<FGameParameterSourceDynamic>(                                                                                                                                                           \
		TArray<FString>({GET_FUNCTION_NAME_STRING_CHECKED(UGameParameter, GetGameUserSettings), GET_FUNCTION_NAME_STRING_CHECKED(UHorrorSettingsLocal, FunctionOrPropertyName)}))

/**
 * Game audio parameter list
 */
UCLASS()
class HORRORGAME_API UAudioParametersList : public UParameterList
{
	GENERATED_BODY()
public:
	UAudioParametersList();
	void CreateMusicVolumeParam();
	void CreateSFXVolumeParam();

protected:
	virtual void Initialize(ULocalPlayer* InLocalPlayer) override;
};
