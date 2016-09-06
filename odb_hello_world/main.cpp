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
