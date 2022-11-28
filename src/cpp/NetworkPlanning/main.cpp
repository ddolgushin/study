#include <stdlib.h>

#include "NetworkPlanning.h"

int main(int argc, char **argv) {
	NetworkPlan *np = NULL;
	const vector<vector<NetworkNode*>*>* paths = NULL;
	
	setlocale(LC_ALL, "Russian");

	if (argc < 2)
		cerr << "Для запуска программы нужно указать название файла данных.\n";
	else {
		try {
			//np = new NetworkPlan(argv[1]);
			np = new NetworkPlan("0 (1,4) (2,7) (3,2)\n1 (5,2)\n2 (5,1) (4,5)\n3 (7,3)\n4 (8,1) (6,2)\n5 (7,3)\n6 (8,12)\n7 (8,4)\n8", false);

			paths = np->getCriticalPaths();

			int size = paths->size();

			cout << "Критическое время: " << np->getTCritical() << endl;
			cout << "Критические пути:" << endl;

			for (int i = 0; i < size; i++) {
				int size2 = paths->at(i)->size();

				cout << "   " << i + 1 << ": ";

				for (int j = 0; j < size2; j++) {
					cout << paths->at(i)->at(j)->toString();

					if (j < size2 - 1) cout << " => ";
				}

				cout << endl;
			}

			delete np;
		} catch (NetworkPlan::Exception ex) {
			cerr << ex.getMessage();
		}
	}

	_getch();

	return 0;
}
