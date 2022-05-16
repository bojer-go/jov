
#if	!defined( __ORDERLIST_H__ )
#define	__ORDERLIST_H__


#include <list>
#include <vector>
#include <queue> 
#include <atlstr.h>
#include <windows.h> 
#include <PROCESS.H> 


typedef std::list<void *> LISTNODE;
typedef std::vector<void *> VECTORNODE;
typedef std::queue<void *> QUEUENODE;

typedef struct OrderVectorNode
{
	long key;
	void *data;

	OrderVectorNode *left;
	OrderVectorNode *right;
}ORDERVECTORNODE, *PORDERVECTORNODE;

typedef struct StrOrderVectorNode
{
	CString key;
	void *data;

	StrOrderVectorNode *left;
	StrOrderVectorNode *right;
}STRORDERVECTORNODE, *PSTRORDERVECTORNODE;

class COrderVector
{
public:
	COrderVector();
	~COrderVector();

private:
	VECTORNODE m_vector;
	CRITICAL_SECTION cs;

	BOOL Search(long *Key, PORDERVECTORNODE *Node, int iLeft, int iRight);
public:
    void Lock();
    void UnLock();
    BOOL virtual Search(long Key, PORDERVECTORNODE *Node);
    int FindPos(long Key, PORDERVECTORNODE *Node);
	int GetCount();
    BOOL Insert(long Key, void *Data, bool bReplace = false);
    BOOL Delete(long Key);
    BOOL Delete(long Key, bool Delete);
    int ClearWithFreeData();
};

class CStrOrderVector
{
public:
	CStrOrderVector();
	~CStrOrderVector();

private:
	VECTORNODE m_vector;
	CRITICAL_SECTION cs;

	BOOL Search(CString &Key, long *iPos, PSTRORDERVECTORNODE *Node, int iLeft, int iRight);
public:
    void Lock();
    void UnLock();
    BOOL virtual Search(CString &Key, PSTRORDERVECTORNODE *Node);
    int FindPos(CString &Key, PSTRORDERVECTORNODE *Node);
	PSTRORDERVECTORNODE GetAt(int index);
	int GetCount();
    BOOL Insert(CString &Key, void *Data, bool bReplace = false);
    BOOL Delete(CString &Key);
    BOOL Delete(CString &Key, bool Delete);
    int ClearWithFreeData();
};

#endif //end __ORDERLIST_H__