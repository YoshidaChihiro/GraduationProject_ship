#pragma once
#include<DirectXMath.h>

struct Vector2:public DirectX::XMFLOAT2
{
	Vector2();
	Vector2(const Vector2& vector);
	Vector2(const XMFLOAT2& vector);

	Vector2(const float& x, const float& y);

	Vector2 operator+(Vector2 vector);

	Vector2 operator-(Vector2 vector);

	Vector2 operator*(Vector2 vector);

	Vector2 operator/(Vector2 vector);

	void operator+= (Vector2 vector);

	void operator-= (Vector2 vector);

	void operator*= (Vector2 vector);

	void operator/= (Vector2 vector);

	Vector2 operator+(float f);

	Vector2 operator-(float f);

	Vector2 operator*(float f);

	Vector2 operator/(float f);

	void operator=(float num);

	//í∑Ç≥ÇéÊìæ
	static float Length(Vector2 vector);
	//ê≥ãKâª
	static Vector2 Normalize(Vector2 vector);
	//äOêœ
	static float Cross(Vector2 vector1, Vector2 vector2);
	//ì‡êœ
	static float Dot(Vector2 vector1, Vector2 vector2);
};


struct Vector3:public DirectX::XMFLOAT3
{

#pragma region ââéZéq

	Vector3();
	Vector3(const Vector3& vector);
	Vector3(const XMFLOAT3& vector);
	Vector3(const DirectX::XMVECTOR& vector);
	Vector3(const float num[3]);
	Vector3(const float& num);
	Vector3(const float& x, const float& y, const float& z);

	Vector3 operator+();

	Vector3 operator-();

	//Vector3& operator*(Vector3 vector);

	//Vector3& operator/(Vector3 vector);

	Vector3& operator+= (Vector3 vector);

	Vector3& operator-= (Vector3 vector);

	Vector3& operator*= (Vector3 vector);

	Vector3& operator/= (Vector3 vector);

	bool operator!= (Vector3 vector);
	bool operator!= (float num);

	bool operator== (Vector3 vector);
	bool operator== (float num);


	Vector3& operator+= (float f);

	Vector3& operator-= (float f);

	Vector3& operator*= (float f);

	Vector3& operator/= (float f);


	Vector3& operator=(float num);


	//í∑Ç≥ÇéÊìæ
	float Length();

	//ê≥ãKâª
	Vector3& Normalize();

	//äOêœ
	Vector3 Cross(Vector3 vector);

	//ì‡êœ
	float Dot(Vector3 vector);

	DirectX::XMFLOAT3 ConvertFLOAT3();

	DirectX::XMVECTOR ConvertXMVECTOR();
	
	static Vector3 Lerp(const Vector3& start, const Vector3& end, float lerpAmount);


	//í∑Ç≥ÇéÊìæ
	static float Length(Vector3 vector);

	//ê≥ãKâª
	static Vector3 Normalize(Vector3 vector);

	//äOêœ
	static Vector3 Cross(Vector3 vector1, Vector3 vector2);

	//ì‡êœ
	static float Dot(Vector3 vector1, Vector3 vector2);

	//ãóó£ÇéÊìæ
	static float Distance(Vector3 vector1, Vector3 vector2);
	

	//XMFLOAT3Ç…ïœä∑
	static DirectX::XMFLOAT3 ConvertFLOAT3(const Vector3& vector);
};

const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);

const Vector3 operator/(const Vector3& v, float s);

struct Vector4 :public DirectX::XMFLOAT4
{
	Vector4();
	Vector4(const Vector4& vector);

	Vector4(const float& x, const float& y, const float& z, const float& w);
	Vector4& operator*(Vector4 vector);

	Vector4& operator/(Vector4 vector);

	Vector4& operator+= (Vector4 vector);

	Vector4& operator-= (Vector4 vector);

	Vector4& operator*= (Vector4 vector);

	Vector4& operator/= (Vector4 vector);

	Vector4& operator+= (float f);

	Vector4& operator-= (float f);

	Vector4& operator*= (float f);

	Vector4& operator/= (float f);


	Vector4& operator=(float num);


	//í∑Ç≥ÇéÊìæ
	float Length();

	//ê≥ãKâª
	Vector4& Normalize();

	//äOêœ
	Vector4 Cross(Vector4 vector);

	//ì‡êœ
	float Dot(Vector4 vector);

	DirectX::XMFLOAT3 ConvertFLOAT3();



	//ê√ìIä÷êî
	static Vector4 straight(Vector4 myPos, Vector4 otherPos);

	//í∑Ç≥ÇéÊìæ
	static float Length(Vector4 vector);

	//ê≥ãKâª
	static Vector4 Normalize(Vector4 vector);

	//äOêœ
	static Vector4 Cross(Vector4 vector1, Vector4 vector2);

	//ì‡êœ
	static float Dot(Vector4 vector1, Vector4 vector2);

	//XMFLOAT3Ç…ïœä∑
	static DirectX::XMFLOAT3 ConvertFLOAT3(const Vector4& vector);
};

const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);
