// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/LMAWeaponComponent.h"
#include "Engine/Engine.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true); 
	
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//���������� ��������� ��������� ��������
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	//���������� ��������� ��������� ������
	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	OnHealthChanged(HealthComponent->GetHealth());//���� �������������� ���������� Health
	//������������� �� �������, ������� �������� � ������ ��������� � ������� ������� AddUObject()
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	//������������� �� �������, ������� ������������ ��������� � �������� ��������� � ������� ������� AddUObject()
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
	
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(HealthComponent->IsDead()))//����� ���������� ������ ���� �������� ���. 
	{
		RotationPlayerOnCursor();//����� ��� ���������� �������� �����
	}
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAxis("CameraZoomInOut", this, &ALMADefaultCharacter::CameraZoomInOut);

	PlayerInputComponent->BindAction("SprintRun", IE_Pressed, this, &ALMADefaultCharacter::SprintRun);
	PlayerInputComponent->BindAction("SprintRun", IE_Released, this, &ALMADefaultCharacter::StopSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::ReloadNeeded);

}

void ALMADefaultCharacter::OnDeath() 
{
	CurrentCursor->DestroyRenderState_Concurrent();//������������� ���������� �������� ���������� �����������. 

	PlayAnimMontage(DeathMontage);//������������� �������� ������ ���������
	GetCharacterMovement()->DisableMovement();//���������� �������� � ���������
	SetLifeSpan(5.0f);//�������, ������� ���������� ������ ����� �������� ���-�� �������

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth) 
{
	//������� �� ����� ������ �������� ��� ��������� ���������� ��������
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor() 
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC))
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value) 
{
	AddMovementInput(GetActorRightVector(), Value);
} 

void ALMADefaultCharacter::CameraZoomInOut(float Value)
{

	if (Value > 0 && newZoomF < MaxZoom)
	{
		newZoomF = newZoomF + ZoomSpeed; 
	}
	else if (Value < 0 && newZoomF > MinZoom)
	{
		newZoomF = newZoomF - ZoomSpeed;
	}
	SpringArmComponent->TargetArmLength = newZoomF;

}
//----------- ������ ������ ������� ----------------
void ALMADefaultCharacter::SprintRun() 
{
	if (GetVelocity().Length() >= 1.f)
	{
		GetWorldTimerManager().SetTimer(SprintTimer, this, &ALMADefaultCharacter::DecreaseStamina, SprintCount, true);
		if (GetWorldTimerManager().IsTimerActive(StaminaTimer)) GetWorldTimerManager().ClearTimer(StaminaTimer);
		
		IsSprint = true;
		GetCharacterMovement()->MaxWalkSpeed = 700.0f;
		GetCharacterMovement()->MaxWalkSpeedCrouched = 700.0f;
	}
	
}

void ALMADefaultCharacter::StopSprint()
{	
	if (GetWorldTimerManager().IsTimerActive(SprintTimer))
	{
		GetWorldTimerManager().ClearTimer(SprintTimer);

		if (Stamina < MaxStamina)
		{
			GetWorldTimerManager().SetTimer(StaminaTimer, this, &ALMADefaultCharacter::IncreaseStamina, StaminaCount, true);
		}
	}
	IsSprint = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;
}

void ALMADefaultCharacter::DecreaseStamina() 
{
	
	Stamina = FMath::Clamp(Stamina - WasteStamina, MinStamina, MaxStamina);
	if (FMath::IsNearlyZero(Stamina))
	{
		StopSprint();
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT("Decrease Stamina = %f"), Stamina));
	
}

void ALMADefaultCharacter::IncreaseStamina() 
{
	if (IsSprint == false)
	{
		Stamina = FMath::Clamp(Stamina + AccumulationStamina, MinStamina, MaxStamina);
		if (Stamina == MaxStamina)
		{
			if (GetWorldTimerManager().IsTimerActive(StaminaTimer))
			{
				GetWorldTimerManager().ClearTimer(StaminaTimer);
			}			
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Increase Stamina = %f"), Stamina));	
	}	
}

