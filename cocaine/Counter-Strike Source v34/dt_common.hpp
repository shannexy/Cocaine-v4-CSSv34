#pragma once

class SendProp;

typedef enum
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,

	DPT_NUMSendPropTypes
} SendPropType;

class DVariant
{
public:
	DVariant()
	{
		m_Type = DPT_Float;
	}

	DVariant( float val )
	{
		m_Type = DPT_Float;
		m_Float = val;
	}

	union
	{
		float		m_Float;
		int			m_Int;
		const char*	m_pString;
		void*		m_pData;
		float		m_Vector[ 3 ];
	};

	SendPropType	m_Type;
};