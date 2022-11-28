#include <conio.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "MHA.h"

// Функция для сравнения двух узлов по приоритетам.
bool compareNodes(Node* l, Node* r) {
	return l->priority > r->priority;
};

int main(int argc, char** argv) {
	setlocale(LC_ALL, "Russian");

	try {
		MhaModel mdl(
			"0 \"Выбор легкового автомобиля для перевозок негабаритных грузов\" 11:3 12:1 13:1 14:2\n"
			"11 \"Объём багажника\"        21:600 22:450 23:560\n" // Литры
			"12 \"Цена\"                   21:2 22:3 23:3\n" // Балл: дешевле -- больше
			"13 \"Расход топлива\"				 21:2 22:3 23:1\n" // Балл: экономичнее -- больше
			"14 \"Стоимость обслуживания\" 21:3 22:2 23:1\n" // Балл: дешевле -- больше
			"21 \"Автомобиль 1\"\n"
			"22 \"Автомобиль 2\"\n"
			"23 \"Автомобиль 3\"", true, false);

		vector<Node*> results = mdl.getResults();
		int size = results.size();

		sort(results.begin(), results.end(), compareNodes);
		cout.precision(2);

		for (int i = 0; i < size; i++) {
			Node* n = results.at(i);

			cout << n->priority << " for \"" << n->name << "\"" << endl;
		}
	}
	catch (MhaModel::Exception ex) {
		cerr << ex.getMessage();
	}

	_getch();

	return 0;
}
