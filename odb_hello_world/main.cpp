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

		//insert john into mysql database 
		{
			//create person instance
			person john("John", "Doe", 13);
			transaction t(db->begin());
			//save john instance into database
			john_id = db->persist(john);
			//here john_id is the core index column id of the table person
			
			cout << "John saved to the database!" << endl;
			cout << "john id is " << john_id << endl;
			person zhouyang("Zhou", "Yang", 22);
			db->persist(zhouyang);
			cout << "zhouyang saved to the database!" << endl;
			//commit change 
			t.commit();
			//change will take effect only after commit !
		}
		//print some statistics about all the people in out database 
		//
		{
			transaction t(db->begin());
			person_stat ps(db->query_value<person_stat>());
			cout << endl;
			cout << "count : " << ps.count << endl;
			cout << "min age : " << ps.min_age << endl;
			cout << "max age : " << ps.max_age << endl;
			cout << endl;
			t.commit();
		}
		typedef odb::query<person> query;
		typedef odb::result<person> result;

		//query for person who's age bigger than 10 
		//say hello to these person
		{
			transaction t(db->begin());
			result r(db->query<person>(query::age > 10));
			for (result::iterator i(r.begin()); i != r.end(); i++) {
				cout << "Hello, " << i->first() << " " << i->last() << " !" << endl;
			}
			t.commit();
		}
		//find instance without using id 
		{
			//find zhouyang and increase the age of zhouyang 
			transaction t(db->begin());
			//if find more than one person, the software will abort
			shared_ptr<person> zhouyang(db->query_one<person>(query::first=="Zhou" && query::last=="Yang"));
			if (zhouyang.get() != 0) {
				zhouyang->age(zhouyang->age() + 1);
				db->update(*zhouyang);
			}
			t.commit();
		}
		{
			transaction t(db->begin());
			db->erase<person>(john_id);
			t.commit();
			cout << "erase John from table person" << endl;
		}
		//execute sql to delete * from table person 
		{
			transaction t(db->begin());
			db->execute("delete  from person");
			t.commit();
			cout << "now person contains nothing!" << endl;
		}

	}
	catch (const odb::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}
