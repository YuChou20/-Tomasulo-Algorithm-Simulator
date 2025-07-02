#include<iostream>
#include<string.h>
#include<string>
#include<vector>
#include<fstream>
#include<map>
#include<queue>
#include<sstream>
#include<utility>
#include<iomanip>
//#define cout outfile
using namespace std;
ifstream infile("ques.txt");
ofstream outfile("ans.txt");
string str = "";
char ctemp[256];
int RSAN, RSMN , ADDC , SUBC , MULC , DIVC , cycle , remain , commit  , issueN , RATN , NO = 0;
bool doJob , exceptions;
map<string, int> RF;
map<string,string> RAT;
queue<string> instruction;

struct ROB_data {
	string rf = "", data = "";
	bool isfinish = false;
	int iss = 0, dis = 0, wri = 0, comm = 0, no = 0;
	ROB_data(string Rf, string Data) :rf(Rf), data(Data) {}
};

struct RS_data {
	string opr = " ", des = "", opd1 = "", opd2 = "";
	int iss = 0, dis = 0, wri = 0, no = 0;
	bool isempty = true;
};

vector<ROB_data> ROB;
vector<RS_data> RSA, RSM;
vector<RS_data> ALU;

void readfile() {		//Read instruction data
	while (infile.getline(ctemp,255)) {
		str = ctemp;
		for (int i = 0; i < str.size(); ++i) {
			if (str[i] == ',') {
				str[i] = ' ';
			}
		}
		instruction.push(str);
	}
}

void settings() {		//Set rs size and instruction cycle
	do {
		cout << "Please input RS size(ADD/SUB):";
		cin >> RSAN;
	} while (RSAN <= 0);
	do {
		cout << "Please input RS size(MUL/DIV):";
		cin >> RSMN;
	} while (RSMN <= 0);
	do {
		cout << "Please input ADD cycle:";
		cin >> ADDC;
	} while (ADDC <= 0);
	do {
		cout << "Please input SUB cycle:";
		cin >> SUBC;
	} while (SUBC <= 0);
	do {
		cout << "Please input MUL cycle:";
		cin >> MULC;
	} while (MULC <= 0);
	do {
		cout << "Please input DIV cycle:";
		cin >> DIVC;
	} while (DIVC <= 0);
}

void initialize() {		//Initialize data
	settings();
	remain = instruction.size();
	RSA.resize(RSAN);
	RSM.resize(RSMN);
	cycle = 0;
	commit = 0;
	issueN = 0;
	RATN = 6;
	NO = 0;
	ROB.clear();
	exceptions = false;
	for (int i = 0; i < instruction.size(); ++i) {
		ROB.push_back(ROB_data("",""));
	}
	RS_data rsd;
	RSA.clear();
	for (int i = 0; i <= RSAN; ++i) {
		RSA.push_back(rsd);
	}
	RSM.clear();
	for (int i = 0; i <= RSMN; ++i) {
		RSM.push_back(rsd);
	}
	RF.clear();
	for (int i = 1; i <= 5; ++i) {
		str = "F";
		str += char(i + '0');
		cout << "Please input initial " << str << " value:";
		int tmp;
		cin >> tmp;
		RF.insert(make_pair(str, tmp));
		RAT.insert(make_pair(str, ""));
	}
	ALU.clear();
	ALU.push_back(rsd);
	ALU.push_back(rsd);
}

bool writeRF(){
	if (ROB[commit].isfinish == true) {
		if (RAT[ROB[commit].rf] == "") {	//if instruction finish then commit
			RF[ROB[commit].rf] = stoi(ROB[commit].data);
		}
		ROB[commit].comm = cycle;
		++commit;
		return true;
	}
	return false;
}

void catch_res(string str,string res) {	//write result back to rs that who is need
	for (int i = 1; i <= RSAN; ++i) {	//Write back to RS(ADD/SUB)
		if (RSA[i].opd1 == str) {
			RSA[i].opd1 = res;
		}
		if (RSA[i].opd2 == str) {
			RSA[i].opd2 = res;
		}
	}
	for (int i = 1; i <= RSMN; ++i) {	//Write back to RS(MUL/DIV)
		if (RSM[i].opd1 == str) {
			RSM[i].opd1 = res;
		}
		if (RSM[i].opd2 == str) {
			RSM[i].opd2 = res;
		}
	}
	for (int i = 1; i <= 5; ++i) {
		string ind = "F" + to_string(i);	//Update RAT
		if (RAT[ind] == str) {
			RAT[ind] = "";
		}
	}
}

bool dispatch() {	//Judge if there is empty ALU to calculate
	int next = 0, iss = 99999;	///FIFO policy
	bool isdispatch = false;
	if (ALU[0].isempty == true) {
		for (int i = 1; i <= RSAN; ++i) {
			if (RSA[i].isempty == false && RSA[i].opd1[0] != 'R' && RSA[i].opd2[0] != 'R') {
				if (iss > RSA[i].no) {	//Judge which one can calculate
					iss = RSA[i].no;
					next = i;
				}
			}
		}
		if (next != 0) {
			ALU[0].isempty = false;
			ALU[0].dis = next;
			ROB[RSA[next].no].dis = cycle;
			ALU[0].wri = cycle + ((RSA[next].opr == "+") ? ADDC : SUBC) - 1;	//Guess the time of Write result
			isdispatch = true;
		}
	}
	iss = 9999;
	next = 0;
	if (ALU[1].isempty == true) {
		for (int i = 1; i <= RSMN; ++i) {	//Judge which one can calculate
			if (RSM[i].isempty == false && RSM[i].opd1[0] != 'R' && RSM[i].opd2[0] != 'R') {
				if (iss > RSM[i].no) {
					iss = RSM[i].no;
					next = i;
				}
			}
		}
		if (next != 0) {
			ALU[1].isempty = false;
			ALU[1].dis = next;
			ROB[RSM[next].no].dis = cycle;
			ALU[1].wri = cycle + ((RSM[next].opr == "*") ? MULC : DIVC) - 1;	//Guess the time of Write result
			isdispatch = true;
		}
	}
	if (isdispatch) {
		return true;
	}
	return false;
}

bool issue(){	//Judge have a empty rs or not
	if (instruction.size() > 0) {
		stringstream ss;
		string des, opd1, opd2;
		str = instruction.front();
		while (ss << str) {
			ss >> str >> des >> opd1 >> opd2;
			if (str.substr(0, 3) == "ADD" || str.substr(0, 3) == "SUB" || str.substr(0, 3) == "add" || str.substr(0, 3) == "sub") {
				for (int i = 1; i <= RSAN; ++i) {
					if (RSA[i].isempty == true) {	//Judge there has a empty RS(ADD/SUB)
						RSA[i].no = NO;
						ROB[NO].iss = cycle;
						RSA[i].iss = cycle;
						RSA[i].isempty = false;
						if (RAT[opd1] != "") {
							RSA[i].opd1 = RAT[opd1];
						}
						else {
							RSA[i].opd1 = to_string(RF[opd1]);
						}
						if (RAT.find(opd2) != RAT.end() && RAT[opd2] != "") {	//Judge is I-type or not
							RSA[i].opd2 = RAT[opd2];
						}
						else if(opd2[0] != 'F'){
							RSA[i].opd2 = opd2;
						}
						else {
							RSA[i].opd2 = to_string(RF[opd2]);
						}
						if (str.substr(0, 3) == "ADD" || str.substr(0, 3) == "add") {
							RSA[i].opr = "+";
						}
						else {
							RSA[i].opr = "-";
						}
						RSA[i].des = des;
						str = "RS" +  to_string(i);
						ROB[NO].data = str;
						ROB[NO].rf = des;
						RAT[des] = str;
						++RATN;
						instruction.pop();
						++NO;
						return true;
					}
				}
			}
			else if (str.substr(0, 3) == "MUL" || str.substr(0, 3) == "DIV" || str.substr(0, 3) == "mul" || str.substr(0, 3) == "div") {
				for (int i = 1; i <= RSMN; ++i) {
					if (RSM[i].isempty == true) {	//Judge there has a empty RS(MUL/DIV)
						RSM[i].no = NO;
						RSM[i].iss = cycle;
						ROB[NO].iss = cycle;
						RSM[i].isempty = false;
						if (RAT[opd1] != "") {
							RSM[i].opd1 = RAT[opd1];
						}
						else {
							RSM[i].opd1 = to_string(RF[opd1]);
						}
						if (RAT.find(opd2) != RAT.end() && RAT[opd2] != "") {	//Judge is I-type or not
							RSM[i].opd2 = RAT[opd2];
						}
						else if (opd2[0] != 'F') {
							RSM[i].opd2 = opd2;
						}
						else {
							RSM[i].opd2 = to_string(RF[opd2]);
						}
						if (str.substr(0, 3) == "MUL" || str.substr(0, 3) == "mul") {
							RSM[i].opr = "*";
						}
						else {
							RSM[i].opr = "/";
						}
						RSM[i].des = des;
						str = "RS" + to_string(i + RSAN);
						ROB[NO].data = str;
						ROB[NO].rf = des;
						RAT[des] = str;
						++RATN;
						instruction.pop();
						++NO;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool write_result() {	//Judge instruction in ALU is finish calculating or not and write result back to rs
	int ind = 0;
	if (ALU[0].isempty == false && ALU[0].wri <= cycle) {
		ind = ALU[0].dis;
		int no = RSA[ind].no;
		int res;
		if (RSA[ind].opr == "+") {
			res = stoi(RSA[ind].opd1) + stoi(RSA[ind].opd2);
		}
		else {
			res = stoi(RSA[ind].opd1) - stoi(RSA[ind].opd2);
		}
		str = "RS" + to_string(ind);
		catch_res(str,to_string(res));
		ALU[0].isempty = true;
		ROB[RSA[ind].no].data = to_string(res);
		RSA[ind].isempty = true;
		RSA[ind].opr = " ";	//Clean RS when finish 
		RSA[ind].opd1 = "";
		RSA[ind].opd2 = "";
		ROB[no].wri = cycle;
		ROB[no].isfinish = true;
		return true;
	}
	else if (ALU[1].isempty == false && ALU[1].wri <= cycle) {
		ind = ALU[1].dis;
		int no = RSM[ind].no;
		int res;
		if (RSM[ind].opr == "*") {
			res = stoi(RSM[ind].opd1) * stoi(RSM[ind].opd2);
		}
		else {
			if (stoi(RSM[ind].opd2) != 0) {
				res = stoi(RSM[ind].opd1) / stoi(RSM[ind].opd2);
			}
			else {
				exceptions = true;
				cout << "Exception happened in cycle " << cycle << endl;
				return false;
			}
		}
		str = "RS" + to_string(ind + RSAN);
		catch_res(str, to_string(res));
		ALU[1].isempty = true;
		ROB[RSM[ind].no].data = to_string(res);
		RSM[ind].isempty = true;	//Clean RS when finish 
		RSM[ind].opr = " ";
		RSM[ind].opd1 = "";
		RSM[ind].opd2 = "";
		ROB[no].wri = cycle;
		ROB[no].isfinish = true;
		return true;
	}
	return false;
}

void printStatus() {	//print (RF, RS, ROB) current status
	cout << right;
	cout << "_____________________Cycle " << cycle << "_____________________" << endl << endl;
	cout << "	  ______RF_______" << endl;
	for (int i = 1; i <= 5; ++i) {
		str = "F";
		str  += char(i + '0');
		cout << setw(7) << str << "  |" << setw(12) <<  RF[str] << "   |" << endl;
	}
	cout << "	  _______________" << endl << endl << endl;

	cout << "	  ___RAT___" << endl;
	for (int i = 1; i <= 5; ++i) {
		str = "F";
		str += char(i + '0');
		cout << setw(7) << str << "  |" << setw(7) << RAT[str] << "  |" << endl;
	}
	cout << " 	  _________" << endl << endl << endl;

	cout << "	  ________RS(ADD/SUB)________" << endl;
	for (int i = 1; i <= RSAN; ++i) {
		str = "RS" + to_string(i);
		cout << setw(7) << str << "  |   " << RSA[i].opr << " |" << setw(9) << RSA[i].opd1 << " |" << setw(9) << RSA[i].opd2 << " |" << endl;
	}
	cout << "	  ___________________________" << endl << endl << endl;
	cout << "	BUFFER:  ";
	if (ALU[0].isempty == true) {
		cout << "empty" << endl;
	}
	else {
		cout << "(RS" << ALU[0].dis << ") " << RSA[ALU[0].dis].opd1 << " " << RSA[ALU[0].dis].opr << " " << RSA[ALU[0].dis].opd2 << endl;
	}
	cout << endl << endl;

	cout << "  	  ________RS(MUL/DIV)_________" << endl;
	for (int i = 1; i <= RSMN; ++i) {
		str = "RS" + to_string(i + RSAN);
		cout << setw(7) << str << "  |   " << RSM[i].opr << " |" << setw(9) << RSM[i].opd1 << " |" << setw(9) << RSM[i].opd2 << "  |" << endl;
	}
	cout << "	  ____________________________" << endl << endl << endl;
	cout << "	BUFFER:  ";
	if (ALU[1].isempty == true) {
		cout << "empty" << endl;
	}
	else {
		cout << "(RS" << ALU[1].dis + RSAN << ") " << RSM[ALU[1].dis].opd1 << " " << RSM[ALU[1].dis].opr << " " << RSM[ALU[1].dis].opd2 << endl;
	}
	cout << endl << endl;

	cout << "  	  ___________ROB____________" << endl;
	cout << "  	 |   REG |     VAL |   DONE |" << endl;
	for (int i = 0; i < ROB.size() - instruction.size(); ++i) {
		str = "ins" + to_string(i);
		cout << setw(7) << str << "  |   " << setw(3) << ROB[i].rf << " |"<< setw(8) << ROB[i].data << " |" << setw(6) << ROB[i].isfinish << "  |" << endl;
	}
	cout << "	  __________________________" << endl << endl << endl;
}

void print_result() {		//print the process of the instruction(cycles)
	cout << endl << endl;
	cout << setw(8) << "issue" << setw(12) << "dispatch" << setw(13) << "write back" << setw(8) << "commit" << endl;
	for (int i = 0; i < ROB.size(); ++i) {
		cout << setw(8) << ROB[i].iss << setw(12) << ROB[i].dis << setw(13) << ROB[i].wri << setw(8) << ROB[i].comm << endl;
	}
}


void run() {
	while (commit < ROB.size() && (!exceptions)) {
		++cycle;
		doJob = false;
		doJob = writeRF() || doJob;
		doJob = dispatch() || doJob;
		doJob = issue() || doJob;
		doJob = write_result() || doJob;
		if (doJob && (!exceptions)) {	//update data
			printStatus();
			//print_result();
		}
	}
}

int main() {
	readfile();
	initialize();
	run();
	print_result();
}