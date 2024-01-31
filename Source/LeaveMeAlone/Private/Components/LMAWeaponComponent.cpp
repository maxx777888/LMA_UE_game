// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Components/LMAWeaponComponent.h"
#include "Player/LMADefaultCharacter.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/LMABaseWeapon.h"

// Sets default values for this component's properties
ULMAWeaponComponent::ULMAWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULMAWeaponComponent::Fire() 
{
	auto Player = Cast<ALMADefaultCharacter>(GetOwner());
	if (IsValid(Player))
	{
		if (Weapon && !AnimReloading && Player->IsSprint==false)
		{
			Weapon->Fire();
		}
	}
	
}

void ULMAWeaponComponent::StopFire() 
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}



void ULMAWeaponComponent::ReloadNeeded() 
{
	if (!CanReload()) return;// �������� ����� �� �� ������������ ������
	ACharacter* Character = Cast<ACharacter>(GetOwner()); // ��������� � �������� ���������
	if (Character->GetVelocity().Length() >= 300.f) return; // ���� �������� ����� ������, �� ������ �����������
	Character->PlayAnimMontage(ReloadMontage); // ������ �������� �����������
	Weapon->StopFire();//��������� � ������ ��������
	Weapon->ChangeClip();// ��������� ���-�� ��������
	AnimReloading = true;// ������������� ����, ��� ���� �����������
}

bool ULMAWeaponComponent::GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const
{
	if (Weapon)
	{
		AmmoWeapon = Weapon->GetCurrentAmmoWeapon();
		return true;
	}

	return false;
}


// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();//��������� ������ � ������ ����
	Weapon->noBullets.AddUObject(this, &ULMAWeaponComponent::ReloadNeeded);

	InitAnimNotify();
}


// Called every frame
void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULMAWeaponComponent::SpawnWeapon() //��������� ������ ��������� �� �����
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);//������� ������ �� �����
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			/*����� ������������ ������ � ���������. ������� ����� ��������� ���������: 
				� ������ ���������� �� ������������ ������.
				��������� ������ �������������.
				�������� ������,� �������� ����������� �������������.*/
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
	}
}

void ULMAWeaponComponent::InitAnimNotify() 
{
	if (!ReloadMontage)//�������� ���������� �� ������������� ��� ������������ ������
		return;

	//������� ����������, � ������� ���������� ������ ���� �����������, ��������� ��� ���������� � ��������
	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)//���������� ������ � ������� ������� �����������
	{
		//����������� ������� ����� ������
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			//������������� ����� ������� �� ������ �����������
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh) 
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	if (!Weapon->isClipFull()) 
	{
		return !AnimReloading;
	}
	else
	{
		return false;
	}
}

