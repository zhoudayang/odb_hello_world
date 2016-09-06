## ODB MySQL  hello world程序  ##
#### 1. 链接库和包含路径 ####
- 链接库和包含路径详见项目根目录下文件夹 **odb-mysql**，其中子文件夹的内容简介可以参考 *config.md*

#### 2. 项目配置 ####

##### 1. 配置包含路径：#####

**详见下图：**
![include directory](http://i.imgur.com/PkBoMtd.png)

##### 2. 配置链接路径：#####
**配置见下图：**
![lib directory](http://i.imgur.com/AG3fu0q.png)

##### 3. 配置依赖项：#####
**见下图：**
![dependency](http://i.imgur.com/cEDs8bX.png)
附加依赖项：**odb-mysql-d.lib;odb-d.lib;%(AdditionalDependencies)**

##### 4. 配置预处理器定义: #####
**如下所示：**
![pre processing](http://i.imgur.com/Sg3F94Q.png)
预处理器定义: **WIN32;_DEBUG;_CONSOLE;_CRT_SECURE_NO_DEPRECATE;DATABASE_MYSQL;%(PreprocessorDefinitions)**

#### 3.定义表结构  ####
参照[官方文档](http://www.codesynthesis.com/products/odb/doc/odb-manual.pdf)，定义如下类，对应表person。

```

#ifndef PERSON_HXX
#define PERSON_HXX

#include <string>
#include <odb/core.hxx>

#pragma db object
class person
{
public:
	person(const std::string& first,
		const std::string& last,
		unsigned short age)
		: first_(first), last_(last), age_(age)
	{
	}

	const std::string&
		first() const
	{
		return first_;
	}

	const std::string&
		last() const
	{
		return last_;
	}

	unsigned short
		age() const
	{
		return age_;
	}

	void
		age(unsigned short age)
	{
		age_ = age;
	}

private:
	friend class odb::access;

	person() {}

#pragma db id auto
	unsigned long id_;

	std::string first_;
	std::string last_;
	unsigned short age_;
};


#endif // PERSON_HXX

```
#### 4.利用odb生成代码 ####

执行命令 ```odb -d mysql --generate-query --generate-schema person.hxx ``` 生成代码，利用生成的*person.sql* 在数据库中创建表。
#### 5.将person类实例保存进入数据库 ####
- 将生成的文件 **person-odb.hxx** 和 **person-odb.ixx** 加入头文件，将**person-odb.cxx**加入源文件。
- 具体实现参照代码，需要指定正确的数据库连接方式，并且数据表**person**已经建立完成。

**main.cpp**
```
#include <iostream>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database.hxx" // create_database

#include "person.hxx"
#include "person-odb.hxx"

using namespace std;
using namespace odb::core;

int main(int argc, char* argv[])
{
	try
	{
		unique_ptr<database> db(new odb::mysql::database(
			"root", "fit123456", "test", "192.168.0.106"
		));
		unsigned long john_id;

		{
			//create person instance
			person john("John", "Doe", 13);
			transaction t(db->begin());
			//save john instance into database
			john_id = db->persist(john);
			//here john_id is the core index column id of the table person
			//commit change 
			t.commit();
			cout << "John saved to the database!" << endl;
			cout << "john id is " << john_id << endl;
		}

	}
	catch (const odb::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}

```
- 为了创建数据库连接，还需要加入下述头文件

**database.hxx**
```
// file      : hello/database.hxx
// copyright : not copyrighted - public domain

//
// Create concrete database instance based on the DATABASE_* macros.
//

#ifndef DATABASE_HXX
#define DATABASE_HXX

#include <string>
#include <memory>   // std::auto_ptr
#include <cstdlib>  // std::exit
#include <iostream>

#include <odb/database.hxx>

#if defined(DATABASE_MYSQL)
#  include <odb/mysql/database.hxx>
#elif defined(DATABASE_SQLITE)
#  include <odb/connection.hxx>
#  include <odb/transaction.hxx>
#  include <odb/schema-catalog.hxx>
#  include <odb/sqlite/database.hxx>
#elif defined(DATABASE_PGSQL)
#  include <odb/pgsql/database.hxx>
#elif defined(DATABASE_ORACLE)
#  include <odb/oracle/database.hxx>
#elif defined(DATABASE_MSSQL)
#  include <odb/mssql/database.hxx>
#else
#  error unknown database; did you forget to define the DATABASE_* macros?
#endif

inline std::auto_ptr<odb::database>
create_database(int& argc, char* argv[])
{
	using namespace std;
	using namespace odb::core;

	if (argc > 1 && argv[1] == string("--help"))
	{
		cout << "Usage: " << argv[0] << " [options]" << endl
			<< "Options:" << endl;

#if defined(DATABASE_MYSQL)
		odb::mysql::database::print_usage(cout);
#elif defined(DATABASE_SQLITE)
		odb::sqlite::database::print_usage(cout);
#elif defined(DATABASE_PGSQL)
		odb::pgsql::database::print_usage(cout);
#elif defined(DATABASE_ORACLE)
		odb::oracle::database::print_usage(cout);
#elif defined(DATABASE_MSSQL)
		odb::mssql::database::print_usage(cout);
#endif

		exit(0);
	}

#if defined(DATABASE_MYSQL)
	auto_ptr<database> db(new odb::mysql::database(argc, argv));
#elif defined(DATABASE_SQLITE)
	auto_ptr<database> db(
		new odb::sqlite::database(
			argc, argv, false, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE));

	// Create the database schema. Due to bugs in SQLite foreign key
	// support for DDL statements, we need to temporarily disable
	// foreign keys.
	//
	{
		connection_ptr c(db->connection());

		c->execute("PRAGMA foreign_keys=OFF");

		transaction t(c->begin());
		schema_catalog::create_schema(*db);
		t.commit();

		c->execute("PRAGMA foreign_keys=ON");
	}
#elif defined(DATABASE_PGSQL)
	auto_ptr<database> db(new odb::pgsql::database(argc, argv));
#elif defined(DATABASE_ORACLE)
	auto_ptr<database> db(new odb::oracle::database(argc, argv));
#elif defined(DATABASE_MSSQL)
	auto_ptr<database> db(new odb::mssql::database(argc, argv));
#endif

	return db;
}

#endif // DATABASE_HXX

```

> [ORM:ODB安装使用过程](http://blog.csdn.net/pamxy/article/details/44839481)
> [ODB学习笔记之基础环境搭建](http://blog.csdn.net/feng______/article/details/33411119)