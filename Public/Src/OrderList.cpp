
#include "stdafx.h"
#include "OrderList.h"

//////////////////////////////////////

COrderVector::COrderVector()
{
	InitializeCriticalSection(&cs);
}

COrderVector::~COrderVector()
{
	ClearWithFreeData();
	DeleteCriticalSection(&cs);
}

void COrderVector::Lock()
{
	EnterCriticalSection(&cs);
}

void COrderVector::UnLock()
{
	LeaveCriticalSection(&cs);
}

int COrderVector::GetCount()
{
	return m_vector.size();
}

BOOL COrderVector::Search(long *Key, PORDERVECTORNODE *Node, int iLeft, int iRight)
{
	int iMid;
	
	if(iRight < iLeft)
	{
		*Node = NULL;
		*Key = -1;
		return false;
	}
	else if(*Key == PORDERVECTORNODE(m_vector[iLeft])->key)
	{
		*Node = (PORDERVECTORNODE)(m_vector[iLeft]);
		*Key = iLeft;
		return true;
	}
	else if(*Key == PORDERVECTORNODE(m_vector[iRight])->key)
	{
		*Node = (PORDERVECTORNODE)m_vector[iRight];
		*Key = iRight;
		return true;
	}
	else if(*Key < PORDERVECTORNODE(m_vector[iLeft])->key)
	{
		*Node = PORDERVECTORNODE(m_vector[iLeft])->left;
		*Key = iLeft - 1;
		return false;
	}
	else if(*Key > PORDERVECTORNODE(m_vector[iRight])->key)
	{
		*Node = PORDERVECTORNODE(m_vector[iRight]);
		*Key = iRight;
		return false;
	}
	else if(iLeft >= iRight -1 )
	{
		*Node = PORDERVECTORNODE(m_vector[iLeft]);
		*Key = iLeft;
		return false;
	}
	else
	{
		iMid = (iLeft + iRight)/2;
		if(*Key < PORDERVECTORNODE(m_vector[iMid])->key)
			return Search(Key, Node, iLeft, iMid -1);
		else if(*Key > PORDERVECTORNODE(m_vector[iMid])->key)
			return Search(Key, Node, iMid, iRight);
		else
		{
			*Node = PORDERVECTORNODE(m_vector[iMid]);
			*Key = iMid;
			return true;
		}
	}
}

BOOL COrderVector::Search(long Key, PORDERVECTORNODE *Node)
{
	*Node = NULL;
	if(0 == m_vector.size())
		return false;
	else
		return Search(&Key, Node, 0, m_vector.size() - 1);
}

int COrderVector::FindPos(long Key, PORDERVECTORNODE *Node)
{
	long iTmp;
	*Node = NULL;
	iTmp = Key;

	if(0 == m_vector.size())
		return -1;
	else
	{
		if(Search(&iTmp, Node, 0, m_vector.size()-1))
			return iTmp;
		else
			return -1;
	}
}

BOOL COrderVector::Insert(long Key, void *Data, bool bReplace)
{
	PORDERVECTORNODE pTmpNode = NULL;
	PORDERVECTORNODE pNode = NULL;
	long iTmp;
	BOOL result = false;
	iTmp = Key;
	if(!Search(&iTmp, &pTmpNode, 0, m_vector.size() - 1))
	{
		if(NULL == pTmpNode)
		{
			pNode = new ORDERVECTORNODE;
			pNode->data = Data;
			pNode->key = Key;
			pNode->left = NULL;
			if(m_vector.size() > 0)
			{
				pNode->right = PORDERVECTORNODE(m_vector[0]);
				PORDERVECTORNODE(m_vector[0])->left = pNode;
			}
			else
			{
				pNode->right = NULL;
			}
			m_vector.insert(m_vector.begin(), (void *)pNode);
		}
		else
		{
			pNode = new ORDERVECTORNODE;
			pNode->data = Data;
			pNode->key = Key;
			pNode->left = pTmpNode;
			pNode->right = pTmpNode->right;
			pTmpNode->right = pNode;
			if(NULL != pNode->right)
				pNode->right->left = pNode;
			m_vector.insert(m_vector.begin() + iTmp + 1, (void *)pNode);
		}
		result = true;
	}
	else if(bReplace)
	{
		//替换 1.释放原指向的数据 2. 指向新的数据   
		/*if OwnsObjects then
		{
			if Assigned(FProcDisposeNode) then
				FProcDisposeNode(pTmpNode->Data)
			else if Assigned(FObjProcDisposeNode) then
				FObjProcDisposeNode(pTmpNode->Data)
			else
				delete pTmpNode->Data;
		}
		pTmpNode->data = Data; */ 
		result = true;
	}
	return result;
}

BOOL COrderVector::Delete(long Key)
{
	BOOL result = false;
    long iTmp = Key;
	PORDERVECTORNODE pTmpNode = NULL;
	if(Search(&iTmp, &pTmpNode, 0, m_vector.size() - 1))
	{
		if(NULL != pTmpNode->right)
		{
			pTmpNode->right->left = pTmpNode->left;
		}
		if(NULL != pTmpNode->left)
		{
			pTmpNode->left->right = pTmpNode->right;
		}  
		//delete pTmpNode->data;
		delete pTmpNode;
		pTmpNode = NULL;
		m_vector.erase(m_vector.begin() + iTmp);
		result = true;
	}
	return result;
}

BOOL COrderVector::Delete(long Key, bool Delete)
{
	BOOL result = false;
	long iTmp = Key;
	PORDERVECTORNODE pTmpNode = NULL;

	if(Search(&iTmp, &pTmpNode, 0, m_vector.size() - 1))
	{
		if(pTmpNode->right)
			pTmpNode->right->left = pTmpNode->left;
		if(pTmpNode->left)
			pTmpNode->left->right = pTmpNode->right;
		if(Delete)
		{
			/*if Assigned(FProcDisposeNode) then
				FProcDisposeNode(pTmpNode->data)
			else if Assigned(FObjProcDisposeNode) then
				FObjProcDisposeNode(pTmpNode->data)
			else
				delete pTmpNode->data;*/
		}
		//delete pTmpNode->data;
		delete pTmpNode;
		pTmpNode = NULL;
		m_vector.erase(m_vector.begin() + iTmp);
		result = true;
	}
	return result;
}

int COrderVector::ClearWithFreeData()
{
	PORDERVECTORNODE Node = NULL;
	int iCount = m_vector.size();
	for(int i=m_vector.size() -1 ; i>=0; i--)
	{
		Node = PORDERVECTORNODE(m_vector[i]);
		//delete Node->data;
		delete Node;
		Node = NULL;
		m_vector.erase(m_vector.begin() + i);
	}
	return iCount;
}

////////////////////////////////////////////////////////////

CStrOrderVector::CStrOrderVector()
{
	InitializeCriticalSection(&cs);
}

CStrOrderVector::~CStrOrderVector()
{
	ClearWithFreeData();
	DeleteCriticalSection(&cs);
}

void CStrOrderVector::Lock()
{
	EnterCriticalSection(&cs);
}

void CStrOrderVector::UnLock()
{
	LeaveCriticalSection(&cs);
}

int CStrOrderVector::GetCount()
{
	return m_vector.size();
}

BOOL CStrOrderVector::Search(CString &Key, long *iPos, PSTRORDERVECTORNODE *Node, int iLeft, int iRight)
{
	int iMid;
	BOOL result=false;
	
	if(Key.IsEmpty()) return false;
	if(iRight < iLeft)
	{
		*Node = NULL;
		*iPos = -1;
		result = false;
	}
	else if(Key == PSTRORDERVECTORNODE(m_vector[iLeft])->key)
	{
		*Node = (PSTRORDERVECTORNODE)(m_vector[iLeft]);
		*iPos = iLeft;
		result = true;
	}
	else if(Key == PSTRORDERVECTORNODE(m_vector[iRight])->key)
	{
		*Node = (PSTRORDERVECTORNODE)m_vector[iRight];
		*iPos = iRight;
		result = true;
	}
	else if(Key < PSTRORDERVECTORNODE(m_vector[iLeft])->key)
	{
		*Node = PSTRORDERVECTORNODE(m_vector[iLeft])->left;
		*iPos = iLeft - 1;
		result = false;
	}
	else if(Key > PSTRORDERVECTORNODE(m_vector[iRight])->key)
	{
		*Node = PSTRORDERVECTORNODE(m_vector[iRight]);
		*iPos = iRight;
		result = false;
	}
	else if(iLeft >= iRight -1 )
	{
		*Node = PSTRORDERVECTORNODE(m_vector[iLeft]);
		*iPos = iLeft;
		result = false;
	}
	else
	{
		iMid = (iLeft + iRight)/2;
		if(Key < PSTRORDERVECTORNODE(m_vector[iMid])->key)
			result = Search(Key, iPos, Node, iLeft, iMid -1);
		else if(Key > PSTRORDERVECTORNODE(m_vector[iMid])->key)
			result = Search(Key, iPos, Node, iMid, iRight);
		else
		{
			*Node = PSTRORDERVECTORNODE(m_vector[iMid]);
			*iPos = iMid;
			result = true;
		}
	}
	return result;
}

BOOL CStrOrderVector::Search(CString &Key, PSTRORDERVECTORNODE *Node)
{
	long iTmp=-1;
	*Node = NULL;
	if(Key.IsEmpty()) return false;
	if(0 == m_vector.size())
		return false;
	else
		return Search(Key, &iTmp, Node, 0, m_vector.size() - 1);
}

int CStrOrderVector::FindPos(CString &Key, PSTRORDERVECTORNODE *Node)
{
	long iTmp=-1;
	*Node = NULL;

	if(0 == m_vector.size())
		return -1;
	else
	{
		if(Search(Key, &iTmp, Node, 0, m_vector.size()-1))
			return iTmp;
		else
			return -1;
	}
}

PSTRORDERVECTORNODE CStrOrderVector::GetAt(int index)
{
	if((index < 0) || (index > (m_vector.size()))) return NULL;
	return (PSTRORDERVECTORNODE)m_vector[index];
}

BOOL CStrOrderVector::Insert(CString &Key, void *Data, bool bReplace)
{
	PSTRORDERVECTORNODE pTmpNode = NULL;
	PSTRORDERVECTORNODE pNode = NULL;
	long iTmp = -1;
	BOOL result = false;
	
	if(Key.IsEmpty()) return false;
	if(!Search(Key, &iTmp, &pTmpNode, 0, m_vector.size() - 1))
	{
		if(NULL == pTmpNode)
		{
			pNode = new STRORDERVECTORNODE;
			pNode->data = Data;
			pNode->key = Key;
			pNode->left = NULL;
			if(m_vector.size() > 0)
			{
				pNode->right = PSTRORDERVECTORNODE(m_vector[0]);
				PSTRORDERVECTORNODE(m_vector[0])->left = pNode;
			}
			else
			{
				pNode->right = NULL;
			}
			m_vector.insert(m_vector.begin(), (void *)pNode);
		}
		else
		{
			pNode = new STRORDERVECTORNODE;
			pNode->data = Data;
			pNode->key = Key;
			pNode->left = pTmpNode;
			pNode->right = pTmpNode->right;
			pTmpNode->right = pNode;
			if(NULL != pNode->right)
				pNode->right->left = pNode;
			m_vector.insert(m_vector.begin() + iTmp + 1, (void *)pNode);
		}
		result = true;
	}
	else if(bReplace)
	{
		//替换 1.释放原指向的数据 2. 指向新的数据   
		/*if OwnsObjects then
		{
			if Assigned(FProcDisposeNode) then
				FProcDisposeNode(pTmpNode->Data)
			else if Assigned(FObjProcDisposeNode) then
				FObjProcDisposeNode(pTmpNode->Data)
			else
				delete pTmpNode->Data;
		}
		pTmpNode->data = Data; */ 
		result = true;
	}
	else
		result = false;
	return result;
}

BOOL CStrOrderVector::Delete(CString &Key)
{
	BOOL result = false;
    long iTmp = -1;
	PSTRORDERVECTORNODE pTmpNode = NULL;
	if(Search(Key, &iTmp, &pTmpNode, 0, m_vector.size() - 1))
	{
		if(NULL != pTmpNode->right)
		{
			pTmpNode->right->left = pTmpNode->left;
		}
		if(NULL != pTmpNode->left)
		{
			pTmpNode->left->right = pTmpNode->right;
		}
    
		//delete pTmpNode->data;
		delete pTmpNode;
		pTmpNode = NULL;
		m_vector.erase(m_vector.begin() + iTmp);
		result = true;
	}
	return result;
}

BOOL CStrOrderVector::Delete(CString &Key, bool Delete)
{
	BOOL result = false;
	long iTmp = -1;
	PSTRORDERVECTORNODE pTmpNode = NULL;

	if(Search(Key, &iTmp, &pTmpNode, 0, m_vector.size() - 1))
	{
		if(pTmpNode->right)
			pTmpNode->right->left = pTmpNode->left;
		if(pTmpNode->left)
			pTmpNode->left->right = pTmpNode->right;
		if(Delete)
		{
			/*if Assigned(FProcDisposeNode) then
				FProcDisposeNode(pTmpNode->data)
			else if Assigned(FObjProcDisposeNode) then
				FObjProcDisposeNode(pTmpNode->data)
			else
				delete pTmpNode->data;*/
		}
		//delete pTmpNode->data;
		delete pTmpNode;
		pTmpNode = NULL;
		m_vector.erase(m_vector.begin() + iTmp);
		result = true;
	}
	return result;
}

int CStrOrderVector::ClearWithFreeData()
{
	PSTRORDERVECTORNODE Node = NULL;
	int iCount = m_vector.size();
	for(int i=m_vector.size() -1 ; i>=0; i--)
	{
		Node = PSTRORDERVECTORNODE(m_vector[i]);
		//这儿只能释放类占用的内存，类里面分配的空间释放不了，因为没有调用类的析构函数
		//delete Node->data; //这样释放会导致内存泄漏
		delete Node;
		Node = NULL;
		m_vector.erase(m_vector.begin() + i);
	}
	return iCount;
}