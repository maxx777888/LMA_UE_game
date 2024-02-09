// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Weapon/LMABaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

//DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);//��� ����� ���������� ���������� �����������

// Sets default values
ALMABaseWeapon::ALMABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);

}

void ALMABaseWeapon::Fire() 
{
	if (IsCurrentClipEmpty())
	{
		return;
	}
	else
	{
		// ������ ������� ��� ��������
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ALMABaseWeapon::Shoot, FireTimer, true);
	}
		
}

void ALMABaseWeapon::StopFire()
{
	if (GetWorldTimerManager().IsTimerActive(FireTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
	}	
}

void ALMABaseWeapon::ChangeClip() 
{
	if (!isClipFull())
	{
		CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
	}
}

bool ALMABaseWeapon::isClipFull()
{
	if (CurrentAmmoWeapon.Bullets == AmmoWeapon.Bullets)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ALMABaseWeapon::SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd) 
{
	const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
	if (TraceFX)
	{
		TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
	}
}

// Called when the game starts or when spawned
void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;//��������� ��������� ������� � ������ ����
}

void ALMABaseWeapon::Shoot() 
{
	const FTransform SocketTransform = WeaponComponent->GetSocketTransform("Muzzle");//�������� ������������ ������ ����
	const FVector TraceStart = SocketTransform.GetLocation();//������ ����������� ����� �����������
	//�������� ����������� ��������, � ������ ������ ��� ���������� X ������ ������
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;//�������� ����� ����� �����������
	// ���������� ����� ����������, ��������� ������ 1.��������� �� ��� 2.����� ������ ���������� ����� 3. �������� ����� 4. ���� �����
	//5. ����� ����� ����������� ��������� ��� �� �������� ����� 6. ����� ������� ����� ����� ����� 7. ������� 8. ������� �����
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);//������ ���������� ��������
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);//����������� ����� ��������

	FHitResult HitResult;
	//LineTraceSingleByChannel - ������ ���������� � ����������� ����� ����������� � ����� ������ ��������
	//��������� ������: 1. ���������� � ����� ����������� ( ��������� FHitResult) 2. ��������� ����� ������ 3. �������� ����� ������
	//4. ����� �������� � ������� �������� ��� ����� �����������, ��� ��������� ������� ����� ��������������
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	FVector TracerEnd = TraceEnd;

	if (HitResult.bBlockingHit)//�������� �� ���������
	{
		TracerEnd = HitResult.ImpactPoint;
		//���� ������, �� ������������� ����������� ������������ � ������� ������ DrawDebugSphere
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 1.0f);
	}

	SpawnTrace(TraceStart, TracerEnd);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);

	DecrementBullets();
}

void ALMABaseWeapon::DecrementBullets() 
{
	CurrentAmmoWeapon.Bullets--;
	
	//UE_LOG(LogWeapon, Display, TEXT("Bullets = %s"), *FString::FromInt(CurrentAmmoWeapon.Bullets));

	if (IsCurrentClipEmpty())//���� ������ �������� �������� ������, ������������� ������ ��������
	{
		StopFire();
		noBullets.Broadcast();
	}
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

// Called every frame
void ALMABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

