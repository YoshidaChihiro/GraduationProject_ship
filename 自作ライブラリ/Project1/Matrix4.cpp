#include "Matrix4.h"
#include<cmath>
#include <cstring>


Matrix4::Matrix4()
{
}

Matrix4::Matrix4(const float val)
{
	for(int i = 0;i<4;i++)
	{
		for(int j = 0;j<4;j++)
		{
			m[i][j] = val;
		}
	}
}

Matrix4::Matrix4(const DirectX::XMMATRIX& mat)
{
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			m[y][x] = mat.r[y].m128_f32[x];
		}
	}
}

Matrix4::Matrix4(const float m00, const float m01, const float m02, const float m03, const float m10, const float m11, const float m12, const float m13, const float m20, const float m21, const float m22, const float m23, const float m30, const float m31, const float m32, const float m33)
{
	float mat[4][4] =
	{
	 m00, m01, m02, m03,
	 m10, m11, m12, m13,
	 m20, m21, m22, m23,
	 m30, m31, m32, m33
	};
	memcpy(m.data(), mat, sizeof(float) * 4 * 4);
}

DirectX::XMMATRIX Matrix4::convertToXMMATRIX()
{
	return Matrix4::convertToXMMATRIX(*this);
}


//Matrix4::Matrix4(const float val[4][4])
//{
//	memcpy(m, val, sizeof(float) * 4 * 4);
//}


Matrix4 Matrix4::identity()
{
	static const Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;
}

//Matrix4 convert(const DirectX::XMMATRIX& mat)
//{
//	Matrix4 matrix;
//	for (int y = 0; y < 4; y++)
//	{
//		for (int x = 0; x < 4; x++)
//		{
//			matrix.m[y][x] = mat.r[y].m128_f32[x];
//		}
//	}
//	return matrix;
//}

DirectX::XMMATRIX Matrix4::convertToXMMATRIX(const Matrix4& mat)
{
	DirectX::XMMATRIX matrix;

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			matrix.r[y].m128_f32[x] = mat.m[y][x];
		}
	}

	return matrix;
}

Matrix4 Matrix4::scale(const Vector3 & s)
{
	Matrix4 result
	{
		s.x,0.0f,0.0f,0.0f,
		0.0f,s.y,0.0f,0.0f,
		0.0f,0.0f,s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	return result;

}

Matrix4 Matrix4::rotateX(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos,sin,0.0f,
		0.0f,-sin,cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}

Matrix4 Matrix4::rotateY(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		cos ,0.0f,-sin, 0.0f,
		0.0f,1.0f,0.0f,0.0f,
		sin ,0.0f,cos , 0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;

}

Matrix4 Matrix4::rotateZ(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result
	{
		cos ,sin ,0.0f, 0.0f,
		-sin,cos ,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}

Matrix4 Matrix4::translate(const Vector3 & t)
{
	static const Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		t.x,t.y,t.z,1.0f
	};

	return result;
}

Vector3 Matrix4::transform(const Vector3 & v, const Matrix4 & m)
{	
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result
	{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z*m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z*m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z*m.m[2][2] + m.m[3][2]) / w
	};
	return result;
}


const Matrix4 operator*(const Matrix4 & m1, const Matrix4 & m2)
{
	Matrix4 result = m1;
	return result *= m2;
}
const Vector3 operator*(const Vector3 & v, const Matrix4 & m)
{
	return Matrix4::transform(v, m);
}

const Matrix4& operator*=(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result(0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

const bool operator==(const Matrix4& m1, const Matrix4& m2)
{

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (m1.m[i][j] != m2.m[i][j])
				return false;
		}
	}
	return true;
}

