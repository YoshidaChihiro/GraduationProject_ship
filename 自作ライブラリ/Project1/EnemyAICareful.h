#pragma once
#include "IEnemyAI.h"

class EnemyAICareful : public IEnemyAI
{
public:
	static EnemyAICareful* GetInstance();

	virtual Vector3 KeepAwayFromGottenPieces(StandardEnemy* arg_enemy, const Vector3& arg_velocity, const Vector3& arg_currentPos, FieldPiece* arg_recentCalcPiece = nullptr) override;
	virtual Vector3 KeepAwayFromFieldBorder(StandardEnemy* arg_enemy, const Vector3& arg_velocity) override;
	virtual Vector3 AgainstFieldTilt(StandardEnemy* arg_enemy, const Vector3& arg_velocity) override;
	virtual Vector3 ApproachEnergyItem(StandardEnemy* arg_enemy, const Vector3& arg_velocity) override;
	virtual Vector3 ApproachCuttingActor(StandardEnemy* arg_enemy, const Vector3& arg_velocity) override;
	virtual Vector3 ApproachActorInFever(StandardEnemy* arg_enemy, const Vector3& arg_velocity) override;

	virtual bool StartCutOnSafeTiming(StandardEnemy* arg_enemy) override;
	virtual bool StartCutEnoughActivePieces(StandardEnemy* arg_enemy) override;
	virtual bool StartCutIncludeBonus(StandardEnemy* arg_enemy) override;
	virtual bool StartCutReachFever(StandardEnemy* arg_enemy) override;
	virtual bool StartCutKillActorInFever(StandardEnemy* arg_enemy) override;

	virtual EnemyAILabel GetLabel()const override;

private:
	EnemyAICareful() {}
	~EnemyAICareful() {}
	EnemyAICareful(const EnemyAICareful& another) = delete;
	EnemyAICareful& operator=(const EnemyAICareful& another) = delete;

};

