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
			np = new NetworkPlan(argv[1]);

			np->calculate();

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
