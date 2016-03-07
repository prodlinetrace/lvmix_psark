//#pragma pack(push,1)

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <windows.h>
#include <unistd.h>
#include <typeinfo>
#include "prodllng/ProdaNG_DLL.h"
using namespace std;


class Proda                   // begin declaration of the class
{
  public:                    // begin public section
    Proda();     // constructor
    ~Proda();                  // destructor

    int init();
    int login();
    int logout();
    int exit_prodll();
    int set_language();
    int get_verbose() const;
    void set_verbose(int v);
    void set_db_connection_params(std::string db_user, std::string db_pass, std::string db_name);

    int identify_me(std::string wabco_number);

    int get_units();
    int get_process();
    int get_production_line();
    int get_process_step();
    int get_system();
    int get_process_step_parameters();
    int get_test_steps_orderby(int test_step_tbl_orderby);
    int get_test_step_params();
    int get_test_values();
    int print_test_steps(int with_test_values);

    int new_product(std::string serial_numer);
    int new_process_result();
    int new_process_step_result(int operator_id);
    int generate_test_results();
    int add_test_step_result(int test_step_id, int result);
    int csv_feed(std::string csv_file);
    bool file_exists_test(const std::string& name);
    int add_test_step_value(int test_step_id, double r1=0, double r2=0, double r3=0, double r4=0, double r5=0);
	int add_test_data(int test_step_id, int test_step_result_status, double r1=0, double r2=0, double r3=0, double r4=0, double r5=0);
    int set_product();
	int set_process_step_result();
	int set_process_result();


 private:                   // begin private section
    //char * string;
	int verbose;
    RetVal	r;
	dbHandle h1; // database handle
	char db_user[100]; // database user
	char db_pass[100]; // database password
	char db_name[100]; // database name
	idbID language_id; // language id

	char wabco_number[PRODANGDLL_WABCONUMBER_SIZE]; // WABCO number
	char system_name[PRODANGDLL_SYSTEM_NAME_SIZE]; // system name
	char serial_number[PRODANGDLL_SERIALNUMBER_SIZE_LSN]; // serial number

	char component_wabco_number[PRODANGDLL_WABCONUMBER_SIZE]; // component WABCO number
	char component_serial_number[PRODANGDLL_SERIALNUMBER_SIZE_LSN]; // component serial number

	// variables for table with units
	UnitDataPtr *unit_data_tbl; // pointer to table with pointers to UnitData
	int unit_data_cnt; // numer of units
	char unit_name[PRODANGDLL_UNITNAME_SIZE];
	char unit_description[PRODANGDLL_DESCRIPTION_SIZE];
	int i_unit; // variable for "for" loop for table with units

	// variables for process definition
	Identification ident; // structure with results from IdentifyMe function.
	Process process; // process
	ProductionLine production_line; // production line
	ProcessStep process_step; // process step
	System system; // system (production station)

	// process step parameters (parameters for one station)
	ProcessStepParam process_step_param; // ProcessStepParam structure
	ProcessStepParamPtr *process_step_param_tbl; // pointer to table with pointers to ProcessStepParam structure
	int process_step_param_cnt; // number of process step parameters

	// test steps (test steps executed on one station)
	TestStep	test_step; // TestStep structure
	TestStepPtr	*test_step_tbl; // pointer to table with pointers to TestStep structure
	int test_step_cnt; // number of test steps
	int test_step_tbl_orderby = GETTESTSTEPS_ORDERBY_TEST_SEQUENCE; // how to sort test steps. Possible values are GETTESTSTEPS_ORDERBY_TEST_SEQUENCE and GETTESTSTEPS_ORDERBY_TEST_ORDER. test sequence is always defined in Proda while test order can be undefined (=0 for all steps). Should be in configuration file

	// test step parameters (variable for all test steps)
	TestStepParam test_step_param; // TestStepParam structure
	TestStepParamPtr *test_step_param_tbl; //pointer to table with pointers to TestStepParam structure
	int test_step_param_cnt; // number of test step parameters
	TestStepParamPtr **test_step_param_tbl_tbl; //table with pointers to table with pointers to TestStepParam structure
	int *test_step_param_cnt_tbl; // table with number of parameters in each test step

	// test values (variables for all test values in all test steps)
	TestValue test_value; // TestValue structure
	TestValuePtr *test_value_tbl; // pointer to table with pointers to TestValue structure
	int test_value_cnt; // number of test values
	TestValuePtr **test_value_tbl_tbl; // table with pointers to table with pointers to TestValue structure
	int *test_value_cnt_tbl; // table with number of test values in each test step


	// variables for results
	char start_time[PRODANGDLL_STARTTIME_SIZE]; // start time string, format is independend from PC regional settings. It is: DD.MM.YYYY HH24:MI:SS (Oracle format)
	char end_time[PRODANGDLL_ENDTIME_SIZE]; // end time string, format is independend from PC regional settings. It is: DD.MM.YYYY HH24:MI:SS (Oracle format)
	//product
	Product_lsn	product; // structure for product (_lsn = product with long serial number)
	// process result
	ProcessResult process_result;
	// process step result
	ProcessStepResult process_step_result;
	// previous process step result
	ProcessStepResult previous_process_step_result;
	int previous_process_step_result_status;
	idbID previous_process_step_result_mode=1; // strictly mode = 1 (strict)
	// test step result
	TestStepResult test_step_result;
	// test value result
	TestValueResult test_value_result;
	//component
	Product_lsn	component; // structure for component (_lsn = product with losng serial number)
	WabcoPart component_wabco_part;

	// other variables
	int i,j; // variable for "for" loop
	double test_value_min,test_value_max; // test_value.minimum/maximum (limits for test value)
	double test_result; // variable for "measures" (test_value_result.result)
	int	device_status=1; // let's generate status=ok
	int device_repeat=0; // let's generate  "no repeat" status
	int qty_components_in_product = 4;


};

	 // constructor of Proda,
	Proda::Proda()
	{
		verbose = 0;
	// values which should be in configuration file
		strcpy(db_user, "prodang"); // database user - should be in configuration file
		strcpy(db_pass, "wabco"); // database password - should be in configuration file
		strcpy(db_name, "proda"); // database name - should be in configuration file
		language_id = 3; // language id  - should be in configuration file; 3=Polish, 2=English, 1=German; You can download list of languages using function GetLanguages

	// values set in the process (ex. by employee on control panel) or configuration file
		strcpy(wabco_number, "46400620101"); // product WABCO number
		strcpy(system_name, "LV_MIX_process"); // system name
	//	strcpy(system_name,"T001"); // system name
		strcpy(serial_number, "106"); // product serial number
		strcpy(component_wabco_number, "0000000112"); // component WABCO number
		strcpy(component_serial_number, "50004"); // component serial number

		int	device_status=1; // let's generate status=ok
		int device_repeat=0; // let's generate  "no repeat" status
		int qty_components_in_product = 0;

	}

	Proda::~Proda()                 // destructor, just an example
	{
		//delete[] db_user;
		//delete[] db_pass;
		//delete[] db_name;

		ExitProDll();

		//delete unit_data_tbl;
		//delete process_step_param_tbl;
		//delete test_step_tbl;

		FreeStructArray((void **)unit_data_tbl);
		FreeStructArray((void **)process_step_param_tbl);
		FreeStructArray((void **)test_step_tbl);
		for (i=0;i<test_step_cnt;i++){
			test_value_tbl = test_value_tbl_tbl[i];
			test_step_param_tbl = test_step_param_tbl_tbl[i];
			FreeStructArray((void **)test_value_tbl);
			FreeStructArray((void **)test_step_param_tbl);
		}
		/***
		***/
	}

	int Proda::get_verbose() const
		{
		   return verbose;
		}

	void Proda::set_verbose(int v)
	{
	   verbose = v;
	}

	void Proda::set_db_connection_params(std::string user, std::string pass, std::string name)
	{
		strcpy(db_user, user.c_str());
		strcpy(db_pass, pass.c_str());
		strcpy(db_name, name.c_str());
		std::cout << "New DB connection parameters set: " << db_user << "/" << db_pass << "@" << db_name << std::endl;
	}

	inline bool Proda::file_exists_test (const std::string& name) {
		return ( access( name.c_str(), F_OK ) != -1 );
	}

	int Proda::init()
	{
	// initialize the library
		r = InitProDll(1);
		if (verbose) printf("InitProDll: ");
		switch (r){
			case 0:
				if (verbose) printf("ok\n");
				break;
				return 0;
			default:
				printf("InitProDll: ");
				printf("error: %d\n",r); // error
				return 1;
		}
		return 0;
	}

	int Proda::login()
	{
	// login to the database
		r = Login(db_user, db_pass, db_name, &h1);
		switch (r){
			case 0:
				if (verbose) printf("DB Login: ");
				if (verbose) printf("ok\n");
				break;
			case -1017:
				printf("Oracle error: ORA-1017 invalid username/password; logon denied (suggestion: check db_user or db_pass)\n");
				return 0;
			case -12154:
				printf("Oracle error: ORA-12154 TNS: could not resolve the connect identifier specified (suggestion: check db_name, file: tnsnames.ora, file: sqlnet.ora, check ping to Oracle server)\n");
				return 0;
			case -12170:
				printf("Oracle error: ORA-12170 TNS:Connect timeout occurred \n");
				return 0;
			case -12541:
				printf("Oracle error: ORA-12541 TNS:no listener (host exists but is there Oracle DB? Is listener working on port defined in tnsnames.ora? suggestion: check db_name, file: tnsnames.ora)\n");
				return 0;
			default:
				if (r < 0) {
					printf("Oracle error: ORA%d\n",r);
				} else {
					printf("DB Login: ");
					printf("error: %d\n",r); // error
				}
				return 0;
		}
		return 0;
	}

	int Proda::logout()
	{
	// login to the database
		r = Logout(h1); // logout from database
		switch (r){
			case 0:
				if (verbose) printf("DB Logout: ");
				if (verbose) printf("ok\n");
				break;
			default:
				printf("DB Logout: ");
				printf("error: %d\n",r); // error
				return 0;
		}
		return 0;
	}
	int Proda::exit_prodll()
	{
		ExitProDll();
		if (verbose) std::cout << "prodll lib disconnected.\n";
		return 0;
	}

	int Proda::set_language()
	{
	// set language for strings returned from Proda
		r = SetLanguage(h1,language_id);
		printf("SetLanguage: ");
		switch (r){
			case 0:
				printf("ok\n");
				break;
			default:
				printf("error: %d\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_units()
	{
	// read all units from database
		r = GetUnits(h1,&unit_data_tbl,&unit_data_cnt);
		printf("GetUnits: ");
		switch (r){
			case 0:
				printf("ok (read %d units)\n",unit_data_cnt);
				break;
			default:
				printf("error: %d\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_process() {
	// read process definition
		r=GetProcess(h1,ident.processId,&process);

		switch (r){
			case 0:
				if (verbose) printf("GetProcess for processId=\"%d\": ",ident.processId);
				if (verbose) printf("ok\n");
				if (verbose) printf(" process.id: %d\n process.productionLineId: %d\n process.releaseId: %d\n process.description: %s\n process.mdReason: %s\n process.mdTime: %s\n process.mdUser: %s\n\n",
						 process.id,      process.productionLineId,      process.releaseId,      process.description,      process.mdReason,      process.mdTime,      process.mdUser);
				break;
			case 1403:
				printf("GetProcess for processId=\"%d\": ",ident.processId);
				printf("error: Record not found\n\n");
				return 0;
			default:
				printf("GetProcess for processId=\"%d\": ",ident.processId);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_production_line() {
	// read production line description, remark: process is executed on production line
		r=GetProductionLine(h1,process.productionLineId,&production_line);

		switch (r){
			case 0:
				if (verbose) printf("GetProductionLine for productionLineId=\"%d\": ",process.productionLineId);
				if (verbose) printf("ok\n");
				if (verbose) printf(" production_line.id: %d\n production_line.description: %s\n\n",
						 production_line.id,      production_line.description);
				break;
			case 1403:
				printf("GetProductionLine for productionLineId=\"%d\": ",process.productionLineId);
				printf("error: Record not found\n\n");
				return 0;
			default:
				printf("GetProductionLine for productionLineId=\"%d\": ",process.productionLineId);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_process_step() {
	// read process step definition
		r=GetProcessStep(h1,ident.processStepId,&process_step);
		switch (r){
			case 0:
				if (verbose) printf("GetProcessStep for processStepId=\"%d\": ",ident.processStepId);
				if (verbose) printf("ok\n");
				if (verbose) printf(" process_step.id: %d\n process_step.processId: %d\n process_step.releaseId: %d\n process_step.systemId: %d\n process_step.processSequence: %d\n process_step.description: %s\n process_step.filename: %s\n process_step.limitRed: %.2f\n process_step.limitYellow: %.2f\n process_step.mdReason: %s\n process_step.mdTime: %s\n process_step.mdUser %s\n\n",
						 process_step.id,      process_step.processId,      process_step.releaseId,      process_step.systemId,      process_step.processSequence,      process_step.description,      process_step.filename,      process_step.limitRed,        process_step.limitYellow,        process_step.mdReason,      process_step.mdTime,      process_step.mdUser);
				break;
			case 1403:
				printf("GetProcessStep for processStepId=\"%d\": ",ident.processStepId);
				printf("error: Record not found\n\n");
				return 0;
			default:
				printf("GetProcessStep for processStepId=\"%d\": ",ident.processStepId);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_system() {
	// read system (production station) name, should be the same like we already define as system_name; remark: process contains multiple process steps, each process step is executed on system
		r=GetSystem(h1,process_step.systemId,&system);
		switch (r){
			case 0:
				if (verbose) printf("GetSystem for systemId=\"%d\": ",process_step.systemId);
				if (verbose) printf("ok\n");
				if (verbose) printf(" system.id: %d\n system.name: %s\n\n",
						 system.id,      system.name);
				break;
			case 1403:
				printf("GetSystem for systemId=\"%d\": ",process_step.systemId);
				printf("error: Record not found\n\n");
				return 0;
			default:
				printf("GetSystem for systemId=\"%d\": ",process_step.systemId);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_process_step_parameters() {
	// read process step parameters
		r=GetProcessStepParams(h1,ident.processStepId,&process_step_param_tbl,&process_step_param_cnt);
		switch (r){
			case 0:
				if (verbose) printf("GetProcessStepParams for processStepId=\"%d\": ",ident.processStepId);
				if (verbose) printf("ok (read %d process step parameters)\n",process_step_param_cnt);
				for (i=0;i<process_step_param_cnt;i++){
					process_step_param = *process_step_param_tbl[i];
					strcpy(unit_name,"");
					strcpy(unit_description,"");
					// find unit name and description
					for (i_unit=0;i_unit<unit_data_cnt;i_unit++){
						if (unit_data_tbl[i_unit]->id == process_step_param_tbl[i]->unitId) {
							strcpy(unit_name,unit_data_tbl[i_unit]->unitName);
							strcpy(unit_description,unit_data_tbl[i_unit]->description);
							break;
						}
					}
					if (verbose) printf(" parameter: %d\n",i+1);
					if (verbose) printf("  process_step_param.id: %d\n  process_step_param.description: %s\n  process_step_param.paramSequence: %d\n  process_step_param.processStepId: %d\n  process_step_param.unitId: %d\n  process_step_param.value: %.2f\n  process_step_param.mdReason: %s\n  process_step_param.mdTime: %s\n  process_step_param.mdUser %s\n",
							  process_step_param.id,       process_step_param.description,       process_step_param.paramSequence,       process_step_param.processStepId,       process_step_param.unitId,       process_step_param.value,         process_step_param.mdReason,       process_step_param.mdTime,       process_step_param.mdUser);
					if (verbose) printf("  unit_name: %s\n  unit_description: %s\n",
							  unit_name,       unit_description);
				}
				if (verbose) printf("\n");
				break;
			default:
				printf("GetProcessStepParams for processStepId=\"%d\": ",ident.processStepId);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_test_steps_orderby(int test_step_tbl_orderby=1) {
	// read test steps
	/*
	GETTESTSTEPS_ORDERBY_TEST_SEQUENCE = 0
	GETTESTSTEPS_ORDERBY_TEST_ORDER = 1
	*/
		r = GetTestSteps_orderby(h1,ident.processStepId,&test_step_tbl,&test_step_cnt,test_step_tbl_orderby);
		switch (r){
			case 0:
				if (verbose) printf("GetTestSteps_orderby for processStepId=\"%d\", orderby=\"%d\": ",ident.processStepId,test_step_tbl_orderby);
				if (verbose) printf("ok (read %d test steps)\n",test_step_cnt);
				for (i=0;i<test_step_cnt;i++){
					test_step = *test_step_tbl[i];
					if (verbose) printf(" step: %d\n",i+1);
					if (verbose) printf("  test_step.id: %d\n  test_step.description: %s\n  test_step.testSequence: %d\n  test_step.testOrder: %d\n  test_step.processStepId: %d\n  test_step.mdReason: %s\n  test_step.mdTime: %s\n  test_step.mdUser %s\n",
							  test_step.id,       test_step.description,       test_step.testSequence,       test_step.testOrder,       test_step.processStepId,       test_step.mdReason,       test_step.mdTime,       test_step.mdUser);
				}
				if (verbose) printf("\n");
				break;
			default:
				printf("GetTestSteps_orderby for processStepId=\"%d\", orderby=\"%d\": ",ident.processStepId,test_step_tbl_orderby);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::get_test_step_params() {
	// read test step parameters (for all test steps)
		test_step_param_cnt_tbl=new int[test_step_cnt]; // initialize table to store number of parameters for each test step
		test_step_param_tbl_tbl=new TestStepParamPtr*[test_step_cnt]; // initialize table to store pointers to tables with pointers to test step parameters
		for (i=0;i<test_step_cnt;i++){
			r = GetTestStepParams(h1,test_step_tbl[i]->id,&(test_step_param_tbl_tbl[i]),&(test_step_param_cnt_tbl[i]));
			switch (r){
				case 0:
					if (verbose) printf("GetTestStepParams for testStepId=\"%d\": ",test_step_tbl[i]->id);
					test_step_param_tbl = test_step_param_tbl_tbl[i];
					test_step_param_cnt = test_step_param_cnt_tbl[i];
					if (verbose) printf("ok (read %d test step parameters)\n",test_step_param_cnt);
					for (j=0;j<test_step_param_cnt;j++){
						test_step_param = *test_step_param_tbl[j];
						strcpy(unit_name,"");
						strcpy(unit_description,"");
						// find unit name and description
						for (i_unit=0;i_unit<unit_data_cnt;i_unit++){
							if (unit_data_tbl[i_unit]->id == test_step_param_tbl[j]->unitId) {
								strcpy(unit_name,unit_data_tbl[i_unit]->unitName);
								strcpy(unit_description,unit_data_tbl[i_unit]->description);
								break;
							}
						}
						if (verbose) printf(" parameter: %d\n",j+1);
						if (verbose) printf("  test_step_param.id: %d\n  test_step_param.description: %s\n  test_step_param.paramSequence: %d\n  test_step_param.testStepId: %d\n  test_step_param.unitId: %d\n  test_step_param.value: %.2f\n  test_step_param.mdReason: %s\n  test_step_param.mdTime: %s\n  test_step_param.mdUser %s\n",
								  test_step_param.id,       test_step_param.description,       test_step_param.paramSequence,       test_step_param.testStepId,       test_step_param.unitId,       test_step_param.value,         test_step_param.mdReason,       test_step_param.mdTime,       test_step_param.mdUser);
						if (verbose) printf("  unit_name: %s\n  unit_description: %s\n",
								  unit_name,       unit_description);
					}
					if (verbose) printf("\n");
					break;
				default:
					printf("GetTestStepParams for testStepId=\"%d\": ",test_step_tbl[i]->id);
					printf("error: %d\n\n",r); // error
					return 0;
			}
		}
		return 0;
	}

	int Proda::get_test_values() {
	// read test values (for all test steps)
		test_value_cnt_tbl=new int[test_step_cnt]; // initialize table to store number of parameters for each test step
		test_value_tbl_tbl=new TestValuePtr*[test_step_cnt]; // initialize table to store pointers to tables with pointers to test values
		for (i=0;i<test_step_cnt;i++){
			r = GetTestValues(h1,test_step_tbl[i]->id,&(test_value_tbl_tbl[i]),&(test_value_cnt_tbl[i]));
			if (verbose) printf("GetTestValues for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
			switch (r){
				case 0:
				test_value_tbl = test_value_tbl_tbl[i];
				test_value_cnt = test_value_cnt_tbl[i];
				if (verbose) printf("ok (read %d test values)\n",test_value_cnt);
					for (j=0;j<test_value_cnt;j++){
						test_value = *test_value_tbl[j];
						strcpy(unit_name,"");
						strcpy(unit_description,"");
						// find unit name and description
						for (i_unit=0;i_unit<unit_data_cnt;i_unit++){
							if (unit_data_tbl[i_unit]->id == test_value_tbl[j]->unitId) {
								strcpy(unit_name,unit_data_tbl[i_unit]->unitName);
								strcpy(unit_description,unit_data_tbl[i_unit]->description);
								break;
							}
						}
						if (verbose) printf(" test value: %d\n",j+1);
						if (verbose) printf("  test_value.id: %d\n  test_value.description: %s\n  test_value.testValueSequence: %d\n  test_value.testStepId: %d\n  test_value.unitId: %d\n  test_value.minimum: %.2f\n  test_value.maximum: %.2f\n  test_value.valueText: %s\n  test_value.mdReason: %s\n  test_value.mdTime: %s\n  test_value.mdUser %s\n",
								  test_value.id,       test_value.description,       test_value.testValueSequence,       test_value.testStepId,       test_value.unitId,       test_value.minimum,         test_value.maximum,         test_value.valueText,       test_value.mdReason,       test_value.mdTime,       test_value.mdUser);
						if (verbose) printf("  unit_name: %s\n  unit_description: %s\n",
								  unit_name,       unit_description);
					}
					if (verbose) printf("\n");
					break;
				default:
					printf("GetTestValues for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
					printf("error: %d\n\n",r); // error
					return 0;
			}
		}
		return 0;
	}

	int Proda::identify_me(std::string wabco_number)
	{
	// identify (find) process, process step etc data based on WABCO number and system name. Function return values into structure Identification ident;
		r = IdentifyMe(h1, system_name, (char *) wabco_number.c_str(), 0, 0, &ident);
		switch (r){
			case 0:
				if (verbose) printf("IdentifyMe for wabco_number=\"%s\" and system_name=\"%s\": ",wabco_number.c_str(),system_name);
				if (verbose) printf("ok\n");
				if (verbose) printf(" ident.processId: %d\n ident.processStepId: %d\n ident.systemId: %d\n ident.wabcoPartId: %d\n\n",
						 ident.processId,      ident.processStepId,      ident.systemId,      ident.wabcoPartId);
				break;
			case ERROR_NO_DATA:
				printf("IdentifyMe for wabco_number=\"%s\" and system_name=\"%s\": ",wabco_number.c_str(),system_name);
				printf("error: ERROR_NO_DATA\n\n");
				return 0;
			default:
				printf("IdentifyMe for wabco_number=\"%s\" and system_name=\"%s\": ",wabco_number.c_str(),system_name);
				printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::new_product(std::string serial_number) {
		/*
		Product generating
			Required: wabcoPartId, serialNumber, individual
			Optional: comment (default=null), crProcessStepId (default=null), crTime (default=sysdate), mdTime (default=crTime)
		*/
		memset(&product, 0, sizeof(product)); // clear product structure
		product.wabcoPartId=ident.wabcoPartId; // set wabco part id
		std::cout << "Adding New Product With Serial Number: " << serial_number.c_str() << "\n";
		//const char * sn = serial_number.c_str();
		strcpy(product.serialNumber, serial_number.c_str()); // serial number for product
		product.individual=1; // set whether product is with SN (individual=1) or without SN (individual=0)
		product.crProcessStepId=ident.processStepId; // set creating process step
		strcpy(product.comment,"added from psark program, v. 1.666"); // comment for product, ex. version of test program

		r = NewProduct_lsn(h1,&product); // create product
		switch (r){
			case 0:
				if (verbose) printf("NewProduct_lsn: ");
				if (verbose) printf("ok\n");
				if (verbose) printf(" New product added\n"); // new product is created
				break;
			case ERROR_MORE_DATA:
				printf("NewProduct_lsn: ");
				printf("ok\n");
				printf(" Product already exists\n"); // this product already exists and NewProduct_lsn function return existing product
				break;
			default:
				printf("NewProduct_lsn: ");
				printf("error: %d\n\n",r); // error
				return 0;
		}
		if (verbose) printf("  product.id: %s\n  product.wabcoPartId: %d\n  product.serialNumber: %s\n  product.individual: %d\n  product.comment: %s\n  product.crProcessStepId: %d\n  product.mdProcessStepId: %d\n  product.crTime: %s\n  product.mdTime: %s\n\n",
				  product.id,       product.wabcoPartId,       product.serialNumber,       product.individual,       product.comment,       product.crProcessStepId,       product.mdProcessStepId,       product.crTime,       product.mdTime);

		return 0;
	}

	int Proda::new_process_result() {

		/*
		Process result generating
			Required: productId, processId, statusId
			Optional: startTime (default=sysdate), endTime (default=null)
		*/
		memset(&process_result, 0 , sizeof(process_result)); // clear process_result structure
		strcpy(process_result.productId, product.id); // set productId for product
		process_result.processId=ident.processId; // set processId for process definition
		process_result.statusId=-1; // -1 -> dll decides about status

		r = NewProcessResult(h1,&process_result);
		switch (r){
			case 0:
				if (verbose) printf("NewProcessResult: ");
				if (verbose) printf("ok\n");
				if (verbose) printf(" New process result added\n");
				break;
			case ERROR_MORE_DATA:
				if (verbose) printf("ok\n");
				if (verbose) printf(" Process result already exists\n");
				break;
			default:
				printf("NewProcessResult: ");
				printf("error: %d\n",r); // error
				return 0;
		}
		if (verbose) printf("  process_result.id: %s\n  process_result.processId: %d\n  process_result.productId: %s\n  process_result.statusId: %d\n  process_result.startTime: %s\n  process_result.endTime: %s\n\n",
				  process_result.id,       process_result.processId,       process_result.productId,       process_result.statusId,       process_result.startTime,       process_result.endTime);

		return 0;
	}

	int Proda::new_process_step_result(int operator_id=1) {
		/*
		Process step result generating
			Required: processStepId, processResultId, statusId
			Optional: startTime (default=sysdate), endTime (default=null), operatorId (default=null)
		*/
		memset(&process_step_result, 0 , sizeof(process_step_result)); // clear process_step_result structure
		process_step_result.processStepId=process_step.id; // set processStepId for process_step definition
		strcpy(process_step_result.processResultId,process_result.id); // set processResultId for process result
		process_step_result.statusId=-1; // -1 -> dll decide about status
		process_step_result.operatorId=operator_id; // set operator id, this is int number. This field gives us possibility to tell to the system who was working on the station to build product

		r = NewProcessStepResult(h1,&process_step_result);
		switch (r){
			case 0:
				if (verbose) printf("NewProcessStepResult: ");
				if (verbose) printf("ok\n");
				if (verbose) printf(" process_step_result.id: %s\n process_step_result.processStepId: %d\n process_step_result.processResultId: %s\n process_step_result.statusId: %d\n process_step_result.startTime: %s\n process_step_result.endTime: %s\n\n",
						 process_step_result.id,      process_step_result.processStepId,      process_step_result.processResultId,      process_step_result.statusId,      process_step_result.startTime,      process_step_result.endTime);
				break;
			default:
				printf("NewProcessStepResult: ");
				printf("error: %d\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::generate_test_results() {

		// test step results generating
		for (i=0;i<test_step_cnt;i++){ // save all test steps
			/*
			Test step result generating
				Required: processStepResultId, testStepId, statusId
			*/
			memset(&test_step_result, 0 , sizeof(test_step_result)); // clear process_step_result structure
			test_step_result.testStepId=test_step_tbl[i]->id; // set testStepId for test step definition
			strcpy(test_step_result.processStepResultId,process_step_result.id); // set processStepResultId for process step result
			test_step_result.statusId=-1; // -1 -> dll decide about status
			r = NewTestStepResult(h1,&test_step_result); // begin test step result creation
			switch (r){
				case 0:
					if (verbose) printf("NewTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
					if (verbose) printf("ok\n");
					if (verbose) printf(" test_step_result.id: %s\n test_step_result.processStepResultId: %s\n test_step_result.testStepId: %d\n test_step_result.statusId: %d\n\n",
							 test_step_result.id,      test_step_result.processStepResultId,      test_step_result.testStepId,      test_step_result.statusId);
					break;
				default:
					printf("NewTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
					printf("error: %d\n\n",r); // error
					return 0;
			}

			// test values for test step
			test_value_tbl = test_value_tbl_tbl[i];
			test_value_cnt = test_value_cnt_tbl[i];
			for (j=0;j<test_value_cnt;j++){
				test_value_min=test_value_tbl[j]->minimum;
				test_value_max=test_value_tbl[j]->maximum;
				if (device_status==1) { // let supppose that device pass the test
					test_result=(double)rand()/(double)RAND_MAX; //0...1
					test_result*=(test_value_max-test_value_min);
					test_result+=test_value_min;
				} else { // let supppose that device didn't pass the test
					test_result=(double)rand()/(double)RAND_MAX; //0...1
					test_result*=(test_value_max-test_value_min)*5;
					test_result+=test_value_min-(test_value_max-test_value_min)*2;
				}
				/*
				Test value generating
					Required: testStepResultId, testValueId, result, statusId
					Optional: contentId (defalt=null)
				*/
				memset(&test_value_result, 0 , sizeof(test_value_result)); // clear test_value_result structure
				test_value_result.result=test_result;
				//std::cout << "RESULT: " << test_value_result.result << "\n";
				test_value_result.statusId=-1; // -1 -> dll decide about status
				//test_value_result.statusId=0; // -1 -> dll decide about status
				test_value_result.testValueId=test_value_tbl[j]->id;
				strcpy(test_value_result.testStepResultId, test_step_result.id);
				r = NewTestValueResult(h1,&test_value_result); // begin test value result creation
				if (verbose) std::cout << "NewTestValueResult for testValueId: "<< test_value_tbl[j]->id <<" Test value sequence: " << test_value_tbl[j]->testValueSequence << " Description: " << test_value_tbl[j]->description << " Min: " << test_value_tbl[j]->minimum << " Max: " << test_value_tbl[j]->maximum << " Val: "<< test_value_result.result<< '\n';

				switch (r){
					case 0:
						if (verbose)printf("ok\n");
						//printf(" test_value_result.id: %s\n test_value_result.testStepResultId: %s\n test_value_result.testValueId: %d\n test_value_result.result: %.3f\n test_value_result.statusId: %d\n\n",
						//		 test_value_result.id,      test_value_result.testStepResultId,      test_value_result.testValueId,      test_value_result.result,        test_value_result.statusId);
						break;
					default:
						printf("error: %d\n\n",r); // error
						return 0;
				}

				/*
				finish test value creation
					Required: id, testStepResultId, testValueId, result, statusId
					Optional: contentId (defalt=null)
					Advise: provide structure returned by NewValueResult; if statusId in NewTestValueResult was already set to correct value (by set -1 or required value) then avoid set it again to -1 to avoid performance drop.
				*/
				r = SetTestValueResult(h1,0,&test_value_result); // finish test value result creation, isRepeat=0 as we do not repeat measurements
				switch (r){
					case 0:
						if (verbose) printf("SetTestValueResult for testValueId=\"%d\"; test value sequence (description): %d (%s): ",test_value_tbl[j]->id,test_value_tbl[j]->testValueSequence,test_value_tbl[j]->description);
						if (verbose) printf("ok\n");
						//printf(" test_value_result.id: %s\n test_value_result.testStepResultId: %s\n test_value_result.testValueId: %d\n test_value_result.result: %.3f\n test_value_result.statusId: %d\n\n",
						//		 test_value_result.id,      test_value_result.testStepResultId,      test_value_result.testValueId,      test_value_result.result,        test_value_result.statusId);
						break;
					default:
						printf("SetTestValueResult for testValueId=\"%d\"; test value sequence (description): %d (%s): ",test_value_tbl[j]->id,test_value_tbl[j]->testValueSequence,test_value_tbl[j]->description);
						printf("error: %d\n\n",r); // error
						return 0;
				}
			}
			/*
			finish test step result creation
				Required: id, processStepResultId, testStepId, statusId
				Advise: provide structure returned by NewTestStepResult and update statusId (-1/-3 to do automatic status calculation or required status).
			*/
			test_step_result.statusId=-1;
			r = SetTestStepResult(h1,0,&test_step_result); // finish test step result creation, isRepeat=0 as we do not repeat steps
			switch (r){
				case 0:
					if (verbose) printf("SetTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
					if (verbose) printf("ok\n");
					if (verbose) printf(" test_step_result.id: %s\n test_step_result.processStepResultId: %s\n test_step_result.testStepId: %d\n test_step_result.statusId: %d\n\n",
							 test_step_result.id,      test_step_result.processStepResultId,      test_step_result.testStepId,      test_step_result.statusId);
					break;
				default:
					printf("SetTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
					printf("error: %d\n\n",r); // error
					return 0;
			}
		}
		return 0;
	}

	int Proda::csv_feed(std::string csv_file) {
		int ts_id, status_id;
		double r1, r2, r3;
		if (csv_file.c_str()) {
			if (file_exists_test(csv_file.c_str())) {
				cout << "Using CSV file: "<< csv_file <<" OK - (file exists)\n";
			} else {
				cerr << "CSV file: " << csv_file << "\n";
				perror("ERROR: specified CSV file does not exists! ");
				return 5;
			}
		}

		int item = 0;
		std::string csv_tokens[5] = {"", "", "", "", ""};
		string line;
		ifstream data(csv_file.c_str());
		if (!data.is_open())
			perror("error while opening file");

		while(std::getline(data, line, '\n')) {
			std::string token;
			std::stringstream  lineStream(line);

			//cout << "Preparing test data to feed: ";
			for (int i=0; std::getline(lineStream, token, ' '); ){
				csv_tokens[i++] = token;
				//cout << token << " ";
			}
			//cout << "\n";
			// convert data to the one accepted by add_test_data
			//ts_id = atoi(csv_tokens[0].c_str());
			// reset_data
			r1=r2=r3=0;
			ts_id = test_step_tbl[item++]->id;  // get the next ts_id defined by database
			status_id = atoi(csv_tokens[0].c_str());
			r1 = atof(csv_tokens[1].c_str());
			r2 = atof(csv_tokens[2].c_str());
			r3 = atof(csv_tokens[3].c_str());
			// feed test data
			std::cout << "Following data will be used to feed: TS_ID: " << ts_id << " STATUS_ID: "<< status_id << " R1: " << r1 << " R2: " << r2 << " R3: " << r3 << std::endl;
			//(int) (int) (double) (double) (double)
			add_test_data(ts_id, status_id, r1, r2, r3);
			//for (int i=0; i< test_step_cnt; i++){ // iterate over test_steps from proda -> save all test steps
			//	/* Required: processStepResultId, testStepId, statusId */
			//	std::cout << test_step_tbl[i]->id << "\n";   // set testStepId for test step definition
			//}
			//csv_line_number++;
			//Psark->add_test_step_result(43, 1);
		}
		if (data.bad())
			perror("error while reading file");
		cout << "CSV feed finished. " << item << " values were uploaded!\n\n";
		return 0;
	}



	int Proda::add_test_data(int test_step_id, int test_step_result_status, double r1=0, double r2=0, double r3=0, double r4=0, double r5=0) {
		/*
			Test step result - adding
			This function should be used to feed database with test data
			Required: processStepResultId, testStepId, statusId, double r1=0, double r2=0, double r3=0, double r4=0, double r5=0
		*/
		double results[5] = {r1, r2, r3, r4, r5};
		int test_step_found = 0;


		memset(&test_step_result, 0 , sizeof(test_step_result)); // clear process_step_result structure
		test_step_result.testStepId=test_step_id; // set testStepId for test step definition
		strcpy(test_step_result.processStepResultId,process_step_result.id); // set processStepResultId for process step result
		test_step_result.statusId=test_step_result_status; // -1 -> dll decide about status
		r = NewTestStepResult(h1,&test_step_result); // begin test step result creation
		//printf("NewTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
		switch (r){
			case 0:
				//std::cout << "ok";
				//std::cout << " tsr.id: " << test_step_result.id <<  " tsr.statusId " << test_step_result.statusId << "\n";
				break;
			default:
				std::cout << "New TRS for TS_ID: " << test_step_id << ". Status: "; //"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
				printf("error: %d\n\n",r); // error
				std::cout << " tsr.id: " << test_step_result.id <<  " tsr.statusId " << test_step_result.statusId << "\n";
				return 0;
		}
		for (i=0;i<test_step_cnt;i++){ // save all test steps
			// test values for test step
			if (test_step_tbl[i]->id == test_step_id) {
				test_step_found = 1;
				//std::cout << "Found TestStep with id: " << test_step_tbl[i]->id << ", desc: " << test_step_tbl[i]->description <<"\n";
				test_value_tbl = test_value_tbl_tbl[i];
				test_value_cnt = test_value_cnt_tbl[i];
				for (j=0;j<test_value_cnt;j++){
					test_value_min=test_value_tbl[j]->minimum;
					test_value_max=test_value_tbl[j]->maximum;

					/*
					Test value generating
						Required: testStepResultId, testValueId, result, statusId
						Optional: contentId (defalt=null)
					*/
					memset(&test_value_result, 0 , sizeof(test_value_result)); // clear test_value_result structure
					test_value_result.result=results[j];
					//std::cout << "RESULT: " << test_value_result.result << "\n";
					test_value_result.statusId=-1; // -1 -> dll decide about status
					//test_value_result.statusId=0; // -1 -> dll decide about status
					test_value_result.testValueId=test_value_tbl[j]->id;
					strcpy(test_value_result.testStepResultId, test_step_result.id);
					r = NewTestValueResult(h1,&test_value_result); // begin test value result creation
					std::cout << "New TVR for TS_ID: " << test_step_id << " TV_ID: "<< test_value_tbl[j]->id <<" TV_SEQ: " << test_value_tbl[j]->testValueSequence << " TV_DESC: " << test_value_tbl[j]->description << " Min: " << test_value_tbl[j]->minimum << " Max: " << test_value_tbl[j]->maximum << " Val: "<< test_value_result.result << " Status: ";

					switch (r){
						case 0:
							printf("ok\n");
							//printf(" test_value_result.id: %s\n test_value_result.testStepResultId: %s\n test_value_result.testValueId: %d\n test_value_result.result: %.3f\n test_value_result.statusId: %d\n\n",
							//		 test_value_result.id,      test_value_result.testStepResultId,      test_value_result.testValueId,      test_value_result.result,        test_value_result.statusId);
							break;
						default:
							printf("error: %d\n\n",r); // error
							return 0;
					}

					/*
					finish test value creation
						Required: id, testStepResultId, testValueId, result, statusId
						Optional: contentId (defalt=null)
						Advise: provide structure returned by NewValueResult; if statusId in NewTestValueResult was already set to correct value (by set -1 or required value) then avoid set it again to -1 to avoid performance drop.
					*/
					r = SetTestValueResult(h1,0,&test_value_result); // finish test value result creation, isRepeat=0 as we do not repeat measurements
					switch (r){
						case 0:
							//printf("ok\n");
							//printf(" test_value_result.id: %s\n test_value_result.testStepResultId: %s\n test_value_result.testValueId: %d\n test_value_result.result: %.3f\n test_value_result.statusId: %d\n\n",
							//		 test_value_result.id,      test_value_result.testStepResultId,      test_value_result.testValueId,      test_value_result.result,        test_value_result.statusId);
							break;
						default:
					std::cout << "Set TVR for TV_ID: " << test_value_tbl[j]->id <<" TV_SEQ: "<< test_value_tbl[j]->testValueSequence <<" TV_DESC: " << test_value_tbl[j]->description << " Status: ";
							printf("error: %d\n",r); // error
							return 0;
					}
				}
			}
				//break; // Test step found - no need to iterate further.
		}
		if (test_step_found == 0) {
			std::cout << "TestStep with id: " << test_step_id << " not found.\n";
		}
		/*
		finish test step result creation
			Required: id, processStepResultId, testStepId, statusId
			Advise: provide structure returned by NewTestStepResult and update statusId (-1/-3 to do automatic status calculation or required status).
		*/
		test_step_result.statusId=test_step_result_status;
		r = SetTestStepResult(h1,0,&test_step_result); // finish test step result creation, isRepeat=0 as we do not repeat steps
		//printf("SetTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
		switch (r){
			case 0:
				//cout << "ok" << " TSR_ID: " << test_step_result.id << " TSR_STATUS_ID: " << test_step_result.statusId << "\n";
				break;
			default:
				std::cout << "Set TSR for testStepId= " << test_step_id << ". Status: "; //"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
				printf("error: %d\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::add_test_step_result(int test_step_id, int test_step_result_status) {
		/*
		Test step result - adding
			Required: processStepResultId, testStepId, statusId
		*/
		memset(&test_step_result, 0 , sizeof(test_step_result)); // clear process_step_result structure
		test_step_result.testStepId=test_step_id; // set testStepId for test step definition
		strcpy(test_step_result.processStepResultId,process_step_result.id); // set processStepResultId for process step result
		test_step_result.statusId=test_step_result_status; // -1 -> dll decide about status
		r = NewTestStepResult(h1,&test_step_result); // begin test step result creation
		//printf("NewTestStepResult for testStepId=\"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
		std::cout << "NewTestStepResult for testStepId= " << test_step_id << ". Status: "; //"%d\"; test step sequence/order (description): %d/%d (%s): ",test_step_tbl[i]->id,test_step_tbl[i]->testSequence,test_step_tbl[i]->testOrder,test_step_tbl[i]->description);
		switch (r){
			case 0:
				printf("ok\n");
				printf(" test_step_result.id: %s\n test_step_result.processStepResultId: %s\n test_step_result.testStepId: %d\n test_step_result.statusId: %d\n\n",
						 test_step_result.id,      test_step_result.processStepResultId,      test_step_result.testStepId,      test_step_result.statusId);
				break;
			default:
				printf("error: %d\n\n",r); // error
				return 0;
		}

		return 0;
	}

	int Proda::add_test_step_value(int test_step_id, double r1=0, double r2=0, double r3=0, double r4=0, double r5=0) {
		//return 0;
		double results[5] = {r1, r2, r3, r4, r5};
		int test_step_found = 0;
		for (i=0;i<test_step_cnt;i++){ // save all test steps
			// test values for test step
			if (test_step_tbl[i]->id == test_step_id) {
				test_step_found = 1;
				std::cout << "Found TestStep with id: " << test_step_tbl[i]->id << ", desc: " << test_step_tbl[i]->description <<"\n";
				test_value_tbl = test_value_tbl_tbl[i];
				test_value_cnt = test_value_cnt_tbl[i];
				for (j=0;j<test_value_cnt;j++){
					test_value_min=test_value_tbl[j]->minimum;
					test_value_max=test_value_tbl[j]->maximum;

					/*
					Test value generating
						Required: testStepResultId, testValueId, result, statusId
						Optional: contentId (defalt=null)
					*/
					memset(&test_value_result, 0 , sizeof(test_value_result)); // clear test_value_result structure
					test_value_result.result=results[j];
					//std::cout << "RESULT: " << test_value_result.result << "\n";
					test_value_result.statusId=-1; // -1 -> dll decide about status
					//test_value_result.statusId=0; // -1 -> dll decide about status
					test_value_result.testValueId=test_value_tbl[j]->id;
					strcpy(test_value_result.testStepResultId, test_step_result.id);
					r = NewTestValueResult(h1,&test_value_result); // begin test value result creation
					std::cout << "NewTestValueResult for testValueId: "<< test_value_tbl[j]->id <<" Test value sequence: " << test_value_tbl[j]->testValueSequence << " Description: " << test_value_tbl[j]->description << " Min: " << test_value_tbl[j]->minimum << " Max: " << test_value_tbl[j]->maximum << " Val: "<< test_value_result.result<< '\n';

					switch (r){
						case 0:
							printf("ok\n");
							//printf(" test_value_result.id: %s\n test_value_result.testStepResultId: %s\n test_value_result.testValueId: %d\n test_value_result.result: %.3f\n test_value_result.statusId: %d\n\n",
							//		 test_value_result.id,      test_value_result.testStepResultId,      test_value_result.testValueId,      test_value_result.result,        test_value_result.statusId);
							break;
						default:
							printf("error: %d\n\n",r); // error
							return 0;
					}

					/*
					finish test value creation
						Required: id, testStepResultId, testValueId, result, statusId
						Optional: contentId (defalt=null)
						Advise: provide structure returned by NewValueResult; if statusId in NewTestValueResult was already set to correct value (by set -1 or required value) then avoid set it again to -1 to avoid performance drop.
					*/
					r = SetTestValueResult(h1,0,&test_value_result); // finish test value result creation, isRepeat=0 as we do not repeat measurements
					printf("SetTestValueResult for testValueId=\"%d\"; test value sequence (description): %d (%s): ",test_value_tbl[j]->id,test_value_tbl[j]->testValueSequence,test_value_tbl[j]->description);
					switch (r){
						case 0:
							printf("ok\n");
							//printf(" test_value_result.id: %s\n test_value_result.testStepResultId: %s\n test_value_result.testValueId: %d\n test_value_result.result: %.3f\n test_value_result.statusId: %d\n\n",
							//		 test_value_result.id,      test_value_result.testStepResultId,      test_value_result.testValueId,      test_value_result.result,        test_value_result.statusId);
							break;
						default:
							printf("error: %d\n\n",r); // error
							return 0;
					}
				}
			}
				//break; // Test step found - no need to iterate further.
		}
		if (test_step_found == 0) {
			std::cout << "TestStep with id: " << test_step_id << " not found.\n";
		}

		return 0;
	}



	int Proda::set_process_step_result() {

		/*
		finish process step result creation
			Required: id, processStepId, processResultId, statusId, startTime
			Optional: endTime (default=sysdate), operatorId (default=null)
			Advise: provide structure returned by NewProcessStepResult and update statusId (-1/-3 to do automatic status calculation or set required status), start__time, end_time
		*/
		//strcpy(process_step_result.startTime,start_time); // set start time for process step (station)
		//strcpy(process_step_result.endTime,end_time); // set end time for process step (station)
		process_step_result.statusId=-1; // -1 -> dll decide about status
		r = SetProcessStepResult(h1,device_repeat,&process_step_result); // finish process step result creation
		if (verbose)
			printf("SetProcessStepResult: ");
		switch (r){
			case 0:
				if (verbose) printf("ok\n");
				if (verbose) printf(" process_step_result.id: %s\n process_step_result.processStepId: %d\n process_step_result.processResultId: %s\n process_step_result.statusId: %d\n process_step_result.startTime: %s\n process_step_result.endTime: %s\n\n",
						 process_step_result.id,      process_step_result.processStepId,      process_step_result.processResultId,      process_step_result.statusId,      process_step_result.startTime,      process_step_result.endTime);
				break;
			default:
				if (verbose) printf(" process_step_result.id: %s\n process_step_result.processStepId: %d\n process_step_result.processResultId: %s\n process_step_result.statusId: %d\n process_step_result.startTime: %s\n process_step_result.endTime: %s\n\n",
						 process_step_result.id,      process_step_result.processStepId,      process_step_result.processResultId,      process_step_result.statusId,      process_step_result.startTime,      process_step_result.endTime);

				if (verbose) printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::set_process_result() {
		/*
		finish process result creation
			Required: id, productId, processId, statusId, startTime
			Optional: endTime (default=sysdate)
			Advise: provide structure returned by NewProcessResult and update statusId (-1/-3 to do automatic status calculation or 1/2 to set required status).
		*/
		process_result.statusId=-1; // -1 -> dll decide about status
		r = SetProcessResult(h1,&process_result); // finish process result creation
		if (verbose) printf("SetProcessResult: ");
		switch (r){
			case 0:
				if (verbose) printf("ok\n");
				if (verbose) printf(" process_result.id: %s\n process_result.processId: %d\n process_result.productId: %s\n process_result.statusId: %d\n process_result.startTime: %s\n process_result.endTime: %s\n\n",
						 process_result.id,      process_result.processId,      process_result.productId,      process_result.statusId,      process_result.startTime,      process_result.endTime);
				break;
			default:
				if (verbose) printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}


	int Proda::set_product() {
		/*
		finish product creation
			Required: id, wabcoPartId, serialNumber, individual,
			Optional: comment, mdTime (default=sysdate), mdProcessStepId (0=null)
			Advise: provide structure filled by NewProduct without any changes
		*/
		r = SetProduct_lsn(h1,&product); // finish product creation,
		if (verbose) printf("SetProduct_lsn: ");
		switch (r){
			case 0:
				if (verbose) printf("ok\n");
				if (verbose) printf(" product.id: %s\n product.wabcoPartId: %d\n product.serialNumber: %s\n product.individual: %d\n product.comment: %s\n product.crProcessStepId: %d\n product.mdProcessStepId: %d\n product.crTime: %s\n product.mdTime: %s\n\n",
						 product.id,      product.wabcoPartId,      product.serialNumber,      product.individual,      product.comment,      product.crProcessStepId,      product.mdProcessStepId,      product.crTime,      product.mdTime);
				break;
			default:
				if (verbose) printf("error: %d\n\n",r); // error
				return 0;
		}
		return 0;
	}

	int Proda::print_test_steps(int with_test_values=0) {
		for (i=0;i<test_step_cnt;i++){ // save all test steps
			std::cout << i+1 << "/" << test_step_cnt <<" TS_ID: " << test_step_tbl[i]->id <<" TS_SEQ: " << test_step_tbl[i]->testSequence <<" TS_ORDER: " << test_step_tbl[i]->testOrder << " TS_DESC: " << test_step_tbl[i]->description <<"\n";
			// test values for test step
			test_value_tbl = test_value_tbl_tbl[i];
			test_value_cnt = test_value_cnt_tbl[i];
			for (j=0;j<test_value_cnt;j++){
				if ( with_test_values ) {
					std::cout << "\tTV_SEQ: " << test_value_tbl[j]->testValueSequence << " TV_ID: " << test_value_tbl[j]->id << " TV_MAX: " << test_value_tbl[j]->maximum << " TV_MIN: " << test_value_tbl[j]->minimum << " TV_DESC: " << test_value_tbl[j]->description << "\n";
				}
			}
		}
		return 0;
	}

inline bool file_exists_test (const std::string& name) {
	return ( access( name.c_str(), F_OK ) != -1 );
}


int main(int argc, char *argv[]){
	puts("PSARK - Proda Swiss Army Knife.");
	int helpflag = 0;
	int verboseflag = 0;
	char *cvalue = NULL;
	char *wvalue = NULL;
	char *svalue = NULL;
	char *fvalue = NULL;
	char *db_user = NULL;
	char *db_pass = NULL;
	char *db_name = NULL;
	int index;
	int c;

	const char * help_string = "Tool to make some bulk operations on PRODA database by Wabco.\n\n"
		"SYNTAX: psark.exe -w <wabco_number> -s [serial] [-c command] [-h] [-v] [-f file] [-u db_user] [-p db_password] [-d db_name]\n\n"

		"Available options:\n"
		"\t-c command to be used\nPlease use of following commands: \n"
		"\t\tcsv_feed - feeds the test_steps and test_values with data specified in CSV file. Entries for selected serial and wabco_number combination will be added. To be used in conjuction with -f option.\n"
		"\t\tgenerate_test_value_data - generates test value data for given wabco_number and serial combination. WARINING do not use on production database as it may cause unwanted pollution.\n"
		"\t\tnew_tv - adds single test value for selected wabco_number and serial. "
		"\t\tget_tv - connects to db with selected wabco_number and serial and displays sequence of test steps and test values. No data in database is changed."
		"\t-w - <wabco_number> - mandatory - 10 digit string specifying wabco_number. AKA product_type. \n"
		"\t-s - <serial> - mandatory - 6 digit string specifying serial number of product.\n"
		"\t-f - use CSV file with data to load\n"
		"\t-u - username which should be used for database connection. Use together with -p and -d options.\n"
		"\t-p - password which should be used for database connection. Use together with -u and -d options.\n"
		"\t-d - databasename which should be used for database connection. Appropriate database must be configured in c:\\oracle\\tnsnames.ora file. Use together with -u and -p options.\n"
		"\t-h - prints this help message\n"
		"\t-v - verbose mode\n"


		"\n\n\nEXAMPLES:\n"
		"Add new test value for test step_id: 48 test_status: 1 test_value1: 31 test_value2: 40 \n"
		"psark.exe -c new_tv -w 4640062010 -s 123456 48 1 31 40\n\n"
		"Generate test data for given wabco_number and serial:\n"
		"psark.exe -c generate_test_value_data -w 4640062010 -s 123456\n\n"
		"Insert data results from CSV file.\nCSV file format (each line): step_id, test_status, test_value1, test_value2, test_value3\n"
		"psark.exe -c csv_feed -w 4640062010 -s 123456 -f file.csv\n\n"
		"Insert data results from CSV file using custom DB credentials\n"
		"psark.exe -c csv_feed -w 4640062010 -s 123456 -f file.csv -u prodang -p wabco -d vb\n\n"
		"Get test_steps defined in db for given wabco_id\n"
		"psark.exe -c get_test_steps -w 4640062010 -s 11 -u prodang -p wabco -d vb\n\n"
		"Get test_steps and test_values defined in db for given wabco_id\n"
		"psark.exe -c get_test_values -w 4640062010 -s 11 -u prodang -p wabco -d vb \n\n"
		;

	opterr = 0;
	while ((c = getopt (argc, argv, "hvc:w:s:f:u:p:d:")) != -1)
	switch (c)
	{
		case 'h':
			helpflag = 1;
			break;
		case 'v':
			verboseflag = 1;
			break;
		case 'c':
			cvalue = optarg;
			break;
		case 'w':
			wvalue = optarg;
			break;
		case 's':
			svalue= optarg;
			break;
		case 'f':
			fvalue = optarg;
			break;
		case 'u':
			db_user = optarg;
			break;
		case 'p':
			db_pass = optarg;
			break;
		case 'd':
			db_name = optarg;
			break;

		case '?':
			if ((optopt == 'c') or (optopt == 'f') or (optopt == 'u') or (optopt == 'p') or (optopt == 'd') or (optopt == 's')or (optopt == 'w'))
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			abort ();
	}
	if (helpflag) {
		std::cout << help_string;
		return 1;
	}

	if (not cvalue) {
		std::cout << "Command parameter is mandatory. Sorry. Exit.\n";
		return 1;
	}
	if (not wvalue) {
		std::cout << "wabco_numer parameter is mandatory. Sorry. Exit.\n";
		return 1;
	}

	if (not svalue) {
		std::cout << "serial parameter is mandatory. Sorry. Exit.\n";
		return 1;
	}

	if (fvalue and 0 ) {
		if (file_exists_test(fvalue)) {
			cout << "Trying to use csv file: "<< fvalue <<" OK - (file exists)\n";
		}else {
			cerr << "CSV file: " << fvalue << "\n";
			perror("ERROR: specified CSV file does not exists! ");
			return 5;
		}

		string line;
		ifstream data(fvalue);
		if (!data.is_open())
		    perror("error while opening file");

		std::string csv_tokens[5] = {"", "", "", "", ""};
		//int i = 0;

		while(std::getline(data, line, '\n')) {
			std::string token;
			std::stringstream  lineStream(line);
			/*
			while(std::getline(lineStream, token, ' ')) {
				cout << token << ' ';
				csv_tokens[i++] = token;
			}
			*/
			for (int i=0; std::getline(lineStream, token, ' '); ){
				cout << token << " ";
				csv_tokens[i++] = token;
			}

			cout << "\n";
			//for (int i=0; i<5; i++)
			//	cout << csv_tokens[i];
		}
	    if (data.bad())
	        perror("error while reading file");

		return 0;

	}
	std::string args[argc];
	std::string command, wabco_number, serial;
	command = cvalue;
	wabco_number = wvalue;
	serial = svalue;

	int i = 0;
	for (index = optind; index < argc; index++) {
		//printf ("Non-option argument %s\n", argv[index]);
		args[++i] =  (std::string) argv[index];
	}
	std::cout << "Command: " << command << " Wabco_number: " << wabco_number << " Serial: " << serial << "\n";

	// PSARK initialization
	Proda *Psark = new Proda();
	Psark->set_verbose(verboseflag);
	Psark->init();

	// set db connection parametes if defined on commandline
	if (db_user and db_pass and db_name) {
		Psark->set_db_connection_params(db_user, db_pass, db_name);
	}

	if (command == "generate_test_value_data") {
		Psark->login();
		Psark->identify_me(wabco_number);
		Psark->get_process();
		Psark->get_production_line();
		Psark->get_process_step();
		Psark->get_system();
		Psark->get_process_step_parameters();
		Psark->get_test_steps_orderby(1);
		Psark->get_test_step_params();
		Psark->get_test_values();

		Psark->new_product(serial);
		Psark->new_process_result();
		Psark->new_process_step_result();

		Psark->generate_test_results();

		Psark->set_process_step_result();
		Psark->set_process_result();
		Psark->set_product();
		Psark->logout();
	} else if (command == "new_tv") {
		Psark->login();
		Psark->identify_me(wabco_number);
		Psark->get_process();
		Psark->get_production_line();
		Psark->get_process_step();
		Psark->get_system();
		Psark->get_process_step_parameters();
		Psark->get_test_steps_orderby(1);
		Psark->get_test_step_params();
		Psark->get_test_values();

		Psark->new_product(serial);
		Psark->new_process_result();
		Psark->new_process_step_result();
		int ts_id, status_id;
		double r1, r2, r3;
		ts_id = atoi(args[3].c_str());
		status_id = atoi(args[4].c_str());
		r1 = atof(args[5].c_str());
		r2 = atof(args[6].c_str());
		r3 = atof(args[7].c_str());

		//(int) args[4], (double) args[5], (double) args[6], (double) args[7]
		//Psark->add_test_step_result(43, 1);
		Psark->add_test_data(ts_id, status_id, r1, r2, r3);

		Psark->set_process_step_result();
		Psark->set_process_result();
		Psark->set_product();
		Psark->logout();
	} else if (command == "get_test_steps") {
		Psark->login();
		Psark->identify_me(wabco_number);
		Psark->get_process();
		Psark->get_production_line();
		Psark->get_process_step();
		Psark->get_system();
		Psark->get_process_step_parameters();
		Psark->get_test_steps_orderby(1);
		Psark->get_test_step_params();
		Psark->get_test_values();
		Psark->print_test_steps(0);
		Psark->logout();
	} else if (command == "get_test_values") {
		Psark->login();
		Psark->identify_me(wabco_number);
		Psark->get_process();
		Psark->get_production_line();
		Psark->get_process_step();
		Psark->get_system();
		Psark->get_process_step_parameters();
		Psark->get_test_steps_orderby(1);
		Psark->get_test_step_params();
		Psark->get_test_values();
		Psark->print_test_steps(1);
		Psark->logout();
	} else if (command == "csv_feed") {
		Psark->login();
		Psark->identify_me(wabco_number);
		Psark->get_process();
		Psark->get_production_line();
		Psark->get_process_step();
		Psark->get_system();
		Psark->get_process_step_parameters();
		Psark->get_test_steps_orderby(1);
		Psark->get_test_step_params();
		Psark->get_test_values();

		Psark->new_product(serial);
		Psark->new_process_result();
		Psark->new_process_step_result();

		Psark->csv_feed(fvalue);

		Psark->set_process_step_result();
		Psark->set_process_result();
		Psark->set_product();
		Psark->logout();
	} else {
		std::cout <<"Command not recognized: " << command << "\n";
	}
	Psark->exit_prodll();
	std::cout << "PSARK finished. \n";
	//delete Psark;
	return EXIT_SUCCESS;
}
