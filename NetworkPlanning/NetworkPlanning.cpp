#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>

#include "NetworkPlanning.h"

void NetworkNode::defaultConstructor() {
	_Te = 0;
	_Tl = 0;
	_R = 0;
	_number = 0;
	_in = new vector<Link*>();
	_out = new vector<Link*>();
}

string NetworkNode::nodeListToString(const char* title, const vector<NetworkNode*>* data) {
	string res;
	int size = 0;
	ostringstream osstr1;
	ostringstream osstr2;

	if (data != NULL && title != NULL) {
		res += title;
		res += ":\n";

		size = data->size();

		for (int i = 0; i < size; i++) {
			osstr1 << i + 1;
			osstr2 << (*data)[i]->_number;

			res += osstr1.str() + ": " + osstr2.str() + "\n";

			osstr1.clear();
			osstr2.clear();
		}
	}

	return res;
}

vector<NetworkNode*>* NetworkNode::extractNodes(vector<Link*>* links, bool src) {
	int size = links->size();
	vector<NetworkNode*>* res = new vector<NetworkNode*>();

	for (int i = 0; i < size; i++) {
		if (src) res->push_back((*links)[i]->src);
		else res->push_back((*links)[i]->dst);
	}

	return res;
}

NetworkNode::NetworkNode() {
	defaultConstructor();
}

NetworkNode::NetworkNode(int number) {
	defaultConstructor();

	_number = number;
}

NetworkNode::~NetworkNode() {
	_in->clear();
	_out->clear();

	delete _in;
	delete _out;
}

void NetworkNode::setTEarliest(double v) { _Te = v; }
void NetworkNode::setTLatest(double v) { _Tl = v; }
void NetworkNode::setReserve(double v) { _R = v; }

double NetworkNode::getTEarliest() { return _Te; }
double NetworkNode::getTLatest() { return _Tl; }
double NetworkNode::getReserve() { return _R; }

void NetworkNode::setNumber(int number) { this->_number = number; }

int NetworkNode::getNumber() { return this->_number; }

void NetworkNode::addOutgoingNode(NetworkNode* node, double lengthTo) {
	Link* lnk = new Link(lengthTo, this, node);
	int size = node->_in->size();
	bool gotLink = false;

	for (int i = 0; i < size; i++) {
		if (node->_in->at(i)->src == this) {
			gotLink = true;

			break;
		}
	}

	if (!gotLink) {
		node->_in->push_back(lnk);
		_out->push_back(lnk);
	}
}

string NetworkNode::getDescription() {
	string res;

	res.append("Текущий: " + toString() + "\n");
	res.append(nodeListToString("Входящие", getIncomingNodes()));
	res.append(nodeListToString("Исходящие", getOutgoingNodes()));

	return res;
}

string NetworkNode::toString() {
	string res;
	ostringstream osstr1;
	ostringstream osstr2;

	osstr1 << _number;
	osstr2 << _Te;

	res.append(osstr1.str() + " (" + osstr2.str() + ")");

	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void NetworkPlan::extractLayers() {
	int size = 0;
	int size2 = 0;
	int size3 = 0;
	vector<NetworkNode*>* curLayer = new vector<NetworkNode*>();
	vector<NetworkNode*>* nextLayer = NULL;
	NetworkNode* tmpNode = NULL;
	NetworkNode* tmpNode2 = NULL;
	bool noMoreIncoming = false;
	bool isFirst = true;
	bool finishAdded = false;

	while (true) {
		if (isFirst) {
			curLayer->push_back(_S);
			_layers->push_back(curLayer);

			isFirst = false;
		} else if (finishAdded) break;

		nextLayer = new vector<NetworkNode*>(); 
		size = curLayer->size();

		// Исключаем исходящие ссылки из узлов текущего слоя.
		for (int j = 0; j < size; j++) {
			tmpNode = curLayer->at(j);
			size2 = tmpNode->getOutgoingLinks()->size();

			for (int k = 0; k < size2; k++) {
				tmpNode->getOutgoingLinks()->at(k)->excluded = true;
			}
		}

		// Проверяем, есть ли другие входящие работы для узлов,
		// являющихся следующими для узлов текущего слоя.
		for (int j = 0; j < size; j++) {
			tmpNode = curLayer->at(j);
			size2 = tmpNode->getOutgoingLinks()->size();

			for (int k = 0; k < size2; k++) {
				tmpNode2 = tmpNode->getOutgoingLinks()->at(k)->dst;
				size3 = tmpNode2->getIncomingLinks()->size();
				noMoreIncoming = true;

				for (int l = 0; l < size3; l++) {
					if (tmpNode2->getIncomingLinks()->at(l)->excluded != true) {
						noMoreIncoming = false;

						break;
					}
				}

				// Если для узла нет работ, не отмеченных как исключённые,
				// и он не добавлен в следующий слой, добавляем.
				if (noMoreIncoming
					&& find(nextLayer->begin(), nextLayer->end(), tmpNode2) == nextLayer->end()) {
						nextLayer->push_back(tmpNode2);

						if (tmpNode2 == _F) finishAdded = true;
				}
			}
		}

		_layers->push_back(nextLayer);

		curLayer = nextLayer;
	}
}

void NetworkPlan::numberNodes() {
	int size = 0;
	int size2 = 0;
	int number = 0;

	if (_layers != NULL && _layers->size() > 0) {
		size = _layers->size();

		for (int i = 0; i < size; i++) {
			size2 = _layers->at(i)->size();

			for (int j = 0; j < size2; j++) {
				_layers->at(i)->at(j)->setNumber(number++);
			}
		}
	}
}

void NetworkPlan::findCriticalPaths(NetworkNode* cur) {
	// Статическая переменная, инициализируется при первом вызове функции.
	// Является счётчиком путей.
	static int pathNum = 0;
	bool isFirst = true;
	NetworkNode* tmpNode = NULL;
	const vector<NetworkNode*>* nodes = cur->getOutgoingNodes();
	vector<NetworkNode*>::iterator itr;
	vector<NetworkNode*>::iterator itr2;
	int size = nodes->size();

	_critPaths->at(pathNum)->push_back(cur);

	for (int i = 0; i < size; i++) {
		tmpNode = nodes->at(i);

		if (tmpNode->getReserve() == 0) {
			if (isFirst) {
				findCriticalPaths(tmpNode);

				isFirst = false;
			} else {
				_critPaths->push_back(new vector<NetworkNode*>());

				// Получаем итератор на начало текущего пути.
				itr = _critPaths->at(pathNum)->begin();
				// Находим в пути текущий узел...
				itr2 = find(itr, _critPaths->at(pathNum)->end(), cur);

				// ... и копируем вершины в новый путь дублирующуюся часть.
				while (itr++ != (itr2 + 1)) {
					_critPaths->at(pathNum + 1)->push_back(*(itr - 1));
				}

				pathNum++;

				findCriticalPaths(tmpNode);
			}
		}
	}
}

NetworkPlan::NetworkPlan(const char* inputFile) {
	ifstream infile(inputFile);
	istringstream* iss = NULL;
	string* strBuf = NULL;
	int size = 0;
	const int bufSize = 128;
	char* chBuf = new char[bufSize];
	int tmpNum = 0;
	int tmpNum2 = 0;
	int j = 0;
	double tmpLength = 0;
	vector<NodeInfo*> input;
	vector<NumLen*> outLinks;
	vector<int> lengths;
	vector<string> tokens;
	NetworkNode* cur = NULL;
	NodeInfo* inf = NULL;
	NodeInfo* tmpInf = NULL;

	if (inputFile == NULL) {
		throw LoadException("Необходимо указать имя файла с исходными данными");
	} else if (!infile) {
		throw ArgumentException("Не удалось открыть файл исходных данных");
	} else {
		_Tcrit = 0;
		_layers = new vector<vector<NetworkNode*>*>();
		_critPaths = new vector<vector<NetworkNode*>*>();

		// Добавляем вектор для первого критического пути.
		_critPaths->push_back(new vector<NetworkNode*>());

		// Считывание описания сети~
		while (infile.getline(chBuf, bufSize)) {
			iss = new istringstream(chBuf);

			copy(istream_iterator<string>(*iss),
				istream_iterator<string>(),
				back_inserter<vector<string>>(tokens));
			outLinks.clear();

			delete iss;

			for (unsigned int i = 0; i < tokens.size(); i++) {
				if (i == 0) {
					tmpNum = atoi(tokens.at(i).c_str());
				} else {
					iss = new istringstream(tokens.at(i));
					j = 0;

					while (iss->getline(chBuf, bufSize, ',')) {
						strBuf = new string(chBuf);

						if (j++ == 0) {
							tmpNum2 = atoi(strBuf->substr(1, strBuf->length() - 1).c_str());
						} else {
							tmpLength = atof(strBuf->substr(0, strBuf->length() - 1).c_str());
						}

						delete strBuf;
					}

					delete iss;

					outLinks.push_back(new NumLen(tmpNum2, tmpLength));
				}
			}

			input.push_back(new NodeInfo(new NetworkNode(tmpNum), outLinks));

			tokens.clear();
		}

		infile.close();
		//~

		// Установка указателей на начальную и конечную вершины.
		_S = input.at(0)->n;
		_F = input.at(input.size() - 1)->n;

		// Построение сети по считанной информации.
		_total = input.size();

		for (int i = 0; i < _total; i++) {
			inf = input.at(i);
			cur = inf->n;
			size = inf->outLinks.size();

			// Проходим по исходящим номерам, ищем совпадающие
			// узлы и создаём для них ссылки-работы. Попутно проверяем
			// наличие ссылок узлов на самих себя.
			for (int j = 0; j < size; j++) {
				for (int k = 0; k < _total; k++) {
					tmpInf = input.at(k);

					if (inf->outLinks.at(j)->num == tmpInf->n->getNumber()) {
						if (tmpInf->n->getNumber() == inf->n->getNumber()) {
							throw NetworkException("Ошибка исходных данных:"
								" недопустимы ссылки вершин на самих себя");
						} else {
							cur->addOutgoingNode(tmpInf->n, inf->outLinks.at(j)->len);
						}
					}
				}
			}
		}

		extractLayers();
		numberNodes();
	}

	delete[] chBuf;
}

NetworkPlan::~NetworkPlan() {
	_layers->clear();
	_critPaths->clear();

	delete _layers;
	delete _critPaths;
}

void NetworkPlan::calculate() {
	int size = _layers->size();
	int size2 = 0;
	int size3 = 0;
	double tmpD = 0;
	double max = 0;
	double min = 0;
	NetworkNode* tmpNode = NULL;
	NetworkNode* tmpNode2 = NULL;
	const vector<Link*>* tmpVec = NULL;

	// Вычисление раннего времени.
	// Движение по слоям в прямом порядке, от стартовой вершины до финишной.
	// Нулевой слой (стартовое состояние) можно не учитывать.
	for (int i = 1; i < size; i++) {
		size2 = _layers->at(i)->size();

		for (int j = 0; j < size2; j++) {
			// Получаем входящие работы для текущей вершины слоя.
			tmpVec = _layers->at(i)->at(j)->getIncomingLinks();
			size3 = tmpVec->size();
			max = 0;

			// Среди входящих работ ищем одну, проход по которой соответствует
			// максимальному пути.
			if (size3 > 0) {
				max = (tmpVec->at(0)->src != NULL)
					? tmpVec->at(0)->src->getTEarliest()
					: 0;
				max += tmpVec->at(0)->length;

				for (int k = 1; k < size3; k++) {
					// Если есть предшествующая вершина, раннее время
					// рассчитывается как сумма её раннего времени и длины работы.
					// Иначе раннее время соответствует только длине работы.
					tmpD = (tmpVec->at(k)->src != NULL)
						? tmpVec->at(k)->src->getTEarliest()
						: 0;
					tmpD += tmpVec->at(k)->length;

					if (tmpD > max) max = tmpD;
				}
			}

			_layers->at(i)->at(j)->setTEarliest(max);
		}
	}

	_F->setTLatest(_F->getTEarliest());
	_F->setReserve(0);

	// Вычисление позднего времени.
	// Движение по слоям в обратном порядке, от финишной вершины до стартовой.
	for (int i = size - 2; i >= 0; i--) {
		size2 = _layers->at(i)->size();

		for (int j = 0; j < size2; j++) {
			tmpNode2 = _layers->at(i)->at(j);
			tmpVec = tmpNode2->getOutgoingLinks();
			size3 = tmpVec->size();

			if (size3 > 0) {
				min = tmpVec->at(0)->dst->getTLatest() - tmpVec->at(0)->length;

				// По исходящим работам определяем позднее время.
				for (int k = 1; k < size3; k++) {
					tmpNode = tmpVec->at(k)->dst;
					tmpD = tmpVec->at(k)->dst->getTLatest() - tmpVec->at(k)->length;

					if (tmpD < min) min = tmpD;
				}

				tmpNode2->setTLatest(min);
				tmpNode2->setReserve(min - tmpNode2->getTEarliest());
			}
		}
	}

	_S->setTEarliest(0);
	_S->setTLatest(0);
	_S->setReserve(0);

	_Tcrit = _F->getTLatest();

	findCriticalPaths(_S);
}
