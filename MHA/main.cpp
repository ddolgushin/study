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
			"0 \"Цель\" 11:5, 12:5, 13:5, 14:5\n"
			"11 \"Критерий_1\" 21:1, 22:2, 23:3\n"
			"12 \"Критерий_2\" 21:1, 22:2, 23:3\n"
			"13 \"Критерий_3\" 21:1, 22:2, 23:3\n"
			"14 \"Критерий_4\" 21:1, 22:2, 23:3\n"
			"21 \"Альтернатива_1\"\n"
			"22 \"Альтернатива_2\"\n"
			"23 \"Альтернатива_3\"", true, false);

		vector<Node*> results = mdl.getResults();
		int size = results.size();

		sort(results.begin(), results.end(), compareNodes);
		cout.precision(2);

		for (int i = 0; i < size; i++) {
			Node* n = results.at(i);

			cout << n->priority << " для альтернативы " << n->name << endl;
		}
	}
	catch (MhaModel::Exception ex){
		cerr << ex.getMessage();
	}

	_getch();

	return 0;
}
