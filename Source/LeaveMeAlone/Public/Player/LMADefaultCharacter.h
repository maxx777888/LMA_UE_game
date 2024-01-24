// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"
class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();
	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; } //������ ��� ��������� ���������� ��������

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent; //��������� �������� 

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;//��������� �� ���������� �������� ������


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

private:
	float YRotation = -75.0f;
	float ArmLength = 1400.0f;
	float FOV = 55.0f;
	
	void OnDeath();//�������, ��������� �� ��������� �� �������� � ������ ���������
	void OnHealthChanged(float NewHealth);//������� ����������� ���� ��������� ��������� � ��������� �������� ���������

	void RotationPlayerOnCursor();//����� ���������� �������� ����� ��� ��������� ���������

	void MoveForward(float Value);
	void MoveRight(float Value);

	//���������� ������������ � ��������� ������ �� ���������
	void CameraZoomInOut(float value);
	float ZoomSpeed = 50.f;
	float MinZoom = 500.f;
	float MaxZoom = 2800.f;
	float newZoomF = ArmLength;

	
	// ��������� �������
	void SprintRun();
	void SprintStop();
	bool IsSprint;
	float Stamina = MaxStamina;

	const float MaxStamina = 100.f;
	const float MinStamina = 0.f;

	float WasteStamina = 0.50f;
	float AccumulationStamina = 0.10f;
	void DecreaseStamina();
	void IncreaseStamina();
	

};
