/*
 * dbHelper.cpp
 *
 *  Created on: 2011-6-15
 *      Author: root
 */

#include "dbHelper.h"
#include "ConnectionPool.h"
extern ConnectionPool* pool;

bool fetchSQLInt(const char* sSQL,int* value,...){
	char sbuff[MAX_LINE_LEN];
	va_list ap;
	va_start(ap,value);
#ifdef WIN32
	_vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#else
	vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#endif
	va_end(ap);
	bool flag=FALSE;
	OracleConnect* db=pool->get();
	try{
		OracleStream o(1,sbuff,*db);
		o.flush();
		if(!o.eof()){
			o>>*value;
			flag=TRUE;
		}
	}catch(otl_exception& ex){
		hsWriteLog(0,"执行SQL:%s 出现错误:%s",sSQL,ex.msg);
	}
	pool->release(db);
	return flag;
}
bool fetchSQLInt(const char* sSQL,int* value1,int* value2,...){
	char sbuff[MAX_LINE_LEN];
	va_list ap;
	va_start(ap,value2);
#ifdef WIN32
	_vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#else
	vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#endif
	va_end(ap);
	bool flag=FALSE;
	OracleConnect* db=pool->get();
	try{
		OracleStream o(1,sbuff,*db);
		o.flush();
		if(!o.eof()){
			o>>*value1>>*value2;
			flag=TRUE;
		}
	}catch(otl_exception& ex){
		hsWriteLog(0,"执行SQL:%s 出现错误:%s",sSQL,ex.msg);
	}
	pool->release(db);
	return flag;
}

bool fetchSQLDouble(char* sSQL,double* value,...){
	char sbuff[MAX_LINE_LEN];
	va_list ap;
	va_start(ap,value);
#ifdef WIN32
	_vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#else
	vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#endif
	va_end(ap);
	bool flag=FALSE;
	OracleConnect* db=pool->get();
	try{
		OracleStream o(1,sbuff,*db);
		o.flush();
		if(!o.eof()){
			o>>*value;
			flag=TRUE;
		}
	}catch(otl_exception& ex){
		hsWriteLog(0,"执行SQL:%s 出现错误:%s",sSQL,ex.msg);
	}
	pool->release(db);
	return flag;
}

bool fetchSQLStr(char* sSQL,char* value,...){
	char sbuff[MAX_LINE_LEN];
	va_list ap;
	va_start(ap,value);
#ifdef WIN32
	_vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#else
	vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#endif
	va_end(ap);
	bool flag=FALSE;
	OracleConnect* db=pool->get();
	try{
		OracleStream o(1,sbuff,*db);
		o.flush();
		if(!o.eof()){
			o>>value;
			flag=TRUE;
		}
	}catch(otl_exception& ex){
		hsWriteLog(0,"执行SQL:%s 出现错误:%s",sSQL,ex.msg);
	}
	pool->release(db);
	return flag;
}

bool fetchSQLStr2(char* sSQL,char* value1, char* value2, char* value3,...){
	char sbuff[MAX_LINE_LEN];
	va_list ap;
	va_start(ap,value3);
#ifdef WIN32
	_vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#else
	vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#endif
	va_end(ap);
	bool flag=FALSE;
	OracleConnect* db=pool->get();
	try{
		OracleStream o(1,sbuff,*db);
		o.flush();
		if(!o.eof()){
			o>>*value1>>*value2>>*value3;
			flag=TRUE;
		}
	}catch(otl_exception& ex){
		hsWriteLog(0,"执行SQL:%s 出现错误:%s",sSQL,ex.msg);
	}
	pool->release(db);
	return flag;
}
/*执行数据更新操作（比如update,delete,insert等语句）*/
int executeSQL(char* sSQL,...){
	char sbuff[MAX_LINE_LEN];
	va_list ap;
	va_start(ap,sSQL);
#ifdef WIN32
	_vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#else
	vsnprintf(sbuff, MAX_LINE_LEN, sSQL, ap);
#endif
	va_end(ap);
	OracleConnect* db=pool->get();
	int n=0;
	try{
		n=db->direct_exec(sbuff);
		db->commit_nowait();
	}catch(otl_exception& ex){
		hsWriteLog(0,"执行executeSQL:[%s] 出现错误:%s",sbuff,ex.msg);
	}
	pool->release(db);
	return n;
}

