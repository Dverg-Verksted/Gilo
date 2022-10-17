// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BotBase.generated.h"

UCLASS(Abstract)
class HORRORGAME_API ABotBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABotBase();

	/*Указатель на точку вайпоинта для следования*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class AWaypoint* NextWaypoint;

protected:
	virtual void BeginPlay() override;

	/*Переменная для счетчика времени*/
	int32 Time;
	FTimerHandle TimerHandle;

	/*Переменная для задания коэффициента замедления*/
	float SpeedParam;

	/*Переменная для скорости передвижения*/
	float CurrentWalkSpeed;

public:
	/* Инициализация бота из дата ассета */
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void InitFromAsset(const class UBotDataAssetBase* BotAsset);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*Замедление бота*/
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void SetMovementSpeed();

	USkeletalMeshComponent* Mesh;
};
