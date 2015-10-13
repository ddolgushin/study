#include <conio.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "MHA.h"

bool compareNodes(Node* l, Node* r) {
	return l->priority > r->priority;
};

int main(int argc, char **argv) {
	setlocale(LC_ALL, "Russian");

	try {
		MhaModel mdl(
			"0 \"����\" 11:5, 12:5, 13:5, 14:5\n"
			"11 \"��������_1\" 21:1, 22:2, 23:3\n"
			"12 \"��������_2\" 21:1, 22:2, 23:3\n"
			"13 \"��������_3\" 21:1, 22:2, 23:3\n"
			"14 \"��������_4\" 21:1, 22:2, 23:3\n"
			"21 \"������������_1\"\n"
			"22 \"������������_2\"\n"
			"23 \"������������_3\"", true, false);

		vector<Node*> results = mdl.getResults();
		int size = results.size();

		sort(results.begin(), results.end(), compareNodes);
		cout.precision(2);

		for (int i = 0; i < size; i++) {
			Node* n = results.at(i);

			cout << n->priority << " ��� ������������ " << n->name << endl;
		}
	}
	catch (MhaModel::Exception ex){
		cerr << ex.getMessage();
	}

	_getch();

	return 0;
}
