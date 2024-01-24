// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Components/LMAHealthComponent.h"

// Sets default values for this component's properties
ULMAHealthComponent::ULMAHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;//ƒелаем его ложным,
}

bool ULMAHealthComponent::IsDead() const
{
	return Health <= 0.0f;
}

bool ULMAHealthComponent::AddHealth(float NewHealth)
{
	if (IsDead() || IsHealthFull()) return false;
	//ƒобавл€ем здоровье на размер пикапа здорь€, но не больше максимума 
	Health = FMath::Clamp(Health + NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);//«апускаетс€ делегат и сообщает об изменении здоровь€
	return true;
}

bool ULMAHealthComponent::IsHealthFull() const
{	
	//FMath::IsNearlyEqual() ѕровер€ет если значени€ равны и возвращает правду если здоровье полное
	return FMath::IsNearlyEqual(Health, MaxHealth);
}


// Called when the game starts
void ULMAHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	OnHealthChanged.Broadcast(Health);//ƒелегат сообщает об изменении состо€нии здоровь€

	AActor* OwnerComponent = GetOwner();//ѕолучаем владельца компоненита, который получил урон
	if (OwnerComponent)
	{
		OwnerComponent->OnTakeAnyDamage.AddDynamic(this, 
			&ULMAHealthComponent::OnTakeAnyDamage);
	}


	
}

void ULMAHealthComponent::OnTakeAnyDamage(
	AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsDead()) return;

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);//ƒелегат сообщает об изменении состо€нии здоровь€

	if (IsDead())
	{
		OnDeath.Broadcast();//ƒелегат оповещает о смерти персонажа
	}
}



