//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Phys pointer association
//-----------------------------------------------------------------------------
static CUtlMap<void *, void *> s_VPhysPtrMap( 0, 0, DefLessFunc(void *) );


CVPhysPtrSaveRestoreOps g_VPhysPtrSaveRestoreOps;
CVPhysPtrUtlVectorSaveRestoreOps g_VPhysPtrUtlVectorSaveRestoreOps;


//-----------------------------------------------------------------------------
// Phys pointer association
//-----------------------------------------------------------------------------
static void AddPtrAssociation( void *pOldValue, void *pNewValue )
{
	s_VPhysPtrMap.Insert( pOldValue, pNewValue );
}


//-----------------------------------------------------------------------------
// Purpose: Save/load part of CPhysicsEnvironment
//-----------------------------------------------------------------------------
static bool NoPhysSaveFunc( const physsaveparams_t &, void * )
{
	//AssertMsg( 0, "Physics cannot save the specified type" );
	return false;
}

bool CPhysicsEnvironment::Save( const physsaveparams_t &params )
{
	const PhysInterfaceId_t type = params.type;
	Assert( type >= 0 && type < PIID_NUM_TYPES );
	
	static PhysSaveFunc_t saveFuncs[PIID_NUM_TYPES] =
	{
		NoPhysSaveFunc,
		(PhysSaveFunc_t)SavePhysicsObject,
		(PhysSaveFunc_t)SavePhysicsFluidController,
		(PhysSaveFunc_t)SavePhysicsSpring,
		(PhysSaveFunc_t)SavePhysicsConstraintGroup,
		(PhysSaveFunc_t)SavePhysicsConstraint,
		(PhysSaveFunc_t)SavePhysicsShadowController,
		(PhysSaveFunc_t)SavePhysicsPlayerController,
		(PhysSaveFunc_t)SavePhysicsMotionController,
		(PhysSaveFunc_t)SavePhysicsVehicleController,
	};
	
	if ( type >= PIID_UNKNOWN && type < PIID_NUM_TYPES )
	{
		params.pSave->WriteData( (char *)&params.pObject, sizeof(void*) );
		return (*saveFuncs[type])( params, params.pObject );
	}
	return false;
}

static bool NoPhysRestoreFunc( const physrestoreparams_t &, void ** )
{
	//AssertMsg( 0, "Physics cannot save the specified type" );
	return false;
}

CVPhysPtrSaveRestoreOps::CVPhysPtrSaveRestoreOps()
{
}


void CPhysicsEnvironment::PreRestore( const physprerestoreparams_t &params )
{
	g_VPhysPtrSaveRestoreOps.PreRestore();
#if !PLATFORM_64BITS
	for ( int i = 0; i < params.recreatedObjectCount; i++ )
	{
		AddPtrAssociation( params.recreatedObjectList[i].pOldObject, params.recreatedObjectList[i].pNewObject );
	}
#endif
}

bool CPhysicsEnvironment::Restore( const physrestoreparams_t &params )
{
	const PhysInterfaceId_t type = params.type;
	Assert( type >= 0 && type < PIID_NUM_TYPES );
	
	static PhysRestoreFunc_t restoreFuncs[PIID_NUM_TYPES] =
	{
		NoPhysRestoreFunc,
		(PhysRestoreFunc_t)RestorePhysicsObject,
		(PhysRestoreFunc_t)RestorePhysicsFluidController,
		(PhysRestoreFunc_t)RestorePhysicsSpring,
		(PhysRestoreFunc_t)RestorePhysicsConstraintGroup,
		(PhysRestoreFunc_t)RestorePhysicsConstraint,
		(PhysRestoreFunc_t)RestorePhysicsShadowController,
		(PhysRestoreFunc_t)RestorePhysicsPlayerController,
		(PhysRestoreFunc_t)RestorePhysicsMotionController,
		(PhysRestoreFunc_t)RestorePhysicsVehicleController,
	};
	
	if ( type >= PIID_UNKNOWN && type < PIID_NUM_TYPES )
	{
		void *pOldObject;
		params.pRestore->ReadData( (char *)&pOldObject, sizeof(void*), 0 );
		if ( (*restoreFuncs[type])( params, params.ppObject ) )
		{
			AddPtrAssociation( pOldObject, *params.ppObject );
			if ( type == PIID_IPHYSICSOBJECT )
			{
				m_objects.AddToTail( (IPhysicsObject *)(*params.ppObject) );
			}
			return true;
		}
	}
	return false;
}

void CPhysicsEnvironment::PostRestore()
{
	g_VPhysPtrSaveRestoreOps.PostRestore();
}


//-----------------------------------------------------------------------------
// Purpose: Fixes up pointers beteween vphysics objects
//-----------------------------------------------------------------------------

void CVPhysPtrSaveRestoreOps::Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave )
{
	char *pField = (char *)fieldInfo.pField;
	int nObjects = fieldInfo.pTypeDesc->fieldSize;
	for ( int i = 0; i < nObjects; i++ )
	{
		pSave->WriteData( (char*)pField, sizeof(void*) );
		pField += sizeof(void*);
	}
}

//-------------------------------------

void CVPhysPtrSaveRestoreOps::PreRestore()
{
	Assert( s_VPhysPtrMap.Count() == 0 );
}

//-------------------------------------

void CVPhysPtrSaveRestoreOps::Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore )
{
	void **ppField = (void **)fieldInfo.pField;
	int nObjects = fieldInfo.pTypeDesc->fieldSize;

	for ( int i = 0; i < nObjects; i++ )
	{
		pRestore->ReadData( (char *)ppField, sizeof(void*), 0 );

		auto iNewVal = s_VPhysPtrMap.Find( *ppField );
		if ( iNewVal != s_VPhysPtrMap.InvalidIndex() )
		{
			*ppField = s_VPhysPtrMap[iNewVal];
		}
		else
		{
			*ppField = nullptr;
		}

		++ppField;
	}
}

//-------------------------------------

void CVPhysPtrSaveRestoreOps::PostRestore()
{	
	s_VPhysPtrMap.RemoveAll();
	PostRestorePhysicsObject();
	PostRestorePhysicsConstraintGroup();
}

//-----------------------------------------------------------------------------

void CVPhysPtrUtlVectorSaveRestoreOps::Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave )
{
	Assert( fieldInfo.pTypeDesc->fieldSize == 1 );

	VPhysPtrVector *pUtlVector = (VPhysPtrVector*)fieldInfo.pField;
	int nObjects = pUtlVector->Count();

	Assert( nObjects <= INT_MAX );

	int objects = static_cast<int>(nObjects);

	pSave->WriteInt( &objects );
	for ( int i = 0; i < objects; i++ )
	{
		pSave->WriteData( (char*)&pUtlVector->Element(i), sizeof(void*) );
	}
}

void CVPhysPtrUtlVectorSaveRestoreOps::Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore )
{
	Assert( fieldInfo.pTypeDesc->fieldSize == 1 );

	VPhysPtrVector *pUtlVector = (VPhysPtrVector*)fieldInfo.pField;

	int nObjects;
	pRestore->ReadInt( &nObjects );
	pUtlVector->AddMultipleToTail( nObjects );
	for ( int i = 0; i < nObjects; i++ )
	{
		void **ppElem = (void**)(&pUtlVector->Element(i));
		pRestore->ReadData( (char *)ppElem, sizeof(void*), 0 );

		auto iNewVal = s_VPhysPtrMap.Find( *ppElem );
		if ( iNewVal != s_VPhysPtrMap.InvalidIndex() )
		{
			*ppElem = s_VPhysPtrMap[iNewVal];
		}
		else
		{
			*ppElem = nullptr;
		}
	}
}
