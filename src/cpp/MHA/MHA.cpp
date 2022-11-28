#include <iterator>
#include <sstream>
#include <fstream>

#include "MHA.h"

using namespace std;


MhaModel::MhaModel(const char* input, bool normalize, bool isFile) {
	if (isFile)
		createModelFromFile(input, normalize);
	else
		createModelFromString(input, normalize);
}

MhaModel::~MhaModel() {
	clearModel();
}

void MhaModel::clearModel() {
	for (int i = 0; i < _allNodes.size(); i++)
		delete _allNodes.at(i);
}

vector<string> MhaModel::xsplit(string buf) {
	vector<string> res;
	int len = buf.length();
	int pos = 0;
	bool insidePars = false;

	for (int i = 0; i < len; i++) {
		if (i == len - 1) // Конец строки.
			res.push_back(buf.substr(pos + 1, (buf.at(i) == '"' ? i - pos - 1 : i - pos)));
		else if (!insidePars) {
			if (buf[i] == ' ' || buf[i] == '\t') {
				if (i == pos + 1)
					pos = i;
				else {
					res.push_back(buf.substr((pos == 0 ? pos : pos + 1), (pos == 0 ? i - pos : i - pos - 1)));

					pos = i;
				}
			}
			else if (buf[i] == '"') {
				insidePars = true;
				pos = i;
			}
		} else {
			if (buf[i] == '"') {
				res.push_back(buf.substr(pos + 1, i - pos - 1));

				insidePars = false;
				pos = ++i;
			}
		}
	}

	return res;
}

bool MhaModel::parse(vector<char*> lines, vector<NodeDescriptor> &nodes) {
	vector<LinkDescriptor> linkBuf;
	int size = lines.size();
	const int bufSize = 128;

	for (int i = 0; i < size; i++) {
		string buf = lines.at(i);
		vector<string> tokens = xsplit(buf);

		linkBuf.clear();

		string tmpName;
		int tmpNum = 0;
		int tmpNum2 = 0;
		float tmpWeight = 0;

		for (unsigned int i = 0; i < tokens.size(); i++) {
			// Номер узла
			if (i == 0)
				tmpNum = atoi(tokens.at(i).c_str());
			// Название узла
			else if (i == 1)
				tmpName = tokens.at(i);
			// Связи
			else {
				istringstream iss(tokens.at(i));
				int j = 0;
				char* chBuf = new char[bufSize];

				while (iss.getline(chBuf, bufSize, ':')) {
					string* strBuf = new string(chBuf);

					if (j++ == 0)
						tmpNum2 = atoi(strBuf->c_str());
					else
					{
						char c = strBuf->at(strBuf->length() - 1);
						string tmp = (c == ',' ? strBuf->substr(0, strBuf->length() - 1) : *strBuf);
						tmpWeight = atof(tmp.c_str());
					}

					delete strBuf;
				}

				delete chBuf;

				linkBuf.push_back(LinkDescriptor(tmpNum2, tmpWeight));
			}
		}

		nodes.push_back(NodeDescriptor(tmpNum, tmpName, linkBuf));
		tokens.clear();
	}

	return true;
}

bool MhaModel::buildModel(vector<NodeDescriptor> nodes) {
	_allNodes.clear();

	int size = nodes.size();

	// Создаём узлы по количеству считанных описаний.
	for (int i = 0; i < size; i++)
		_allNodes.push_back(new Node(nodes.at(i).num, nodes.at(i).name, i == 0 ? 1 : 0));

	// Устанавливаем указатель на начальную вершину (корень дерева).
	_root = _allNodes.at(0);

	// Формируем дерево, связывая узлы по номерам.
	for (int i = 0; i < size; i++) {
		Node *node = _allNodes.at(i);
		NodeDescriptor nodeDesc = nodes.at(i);
		int linksSize = nodeDesc.links.size();

		for (int j = 0; j < linksSize; j++)
			for (int k = 0; k < size; k++)
				if (nodeDesc.links.at(j).targetNum == _allNodes.at(k)->num)
					node->out.push_back(new Link(_allNodes.at(k), nodeDesc.links.at(j).weight));
	}

	_isModelReady = true;

	return true;
}

void MhaModel::createModel(vector<char*> lines, bool normalize) {
	clearModel();
	
	vector<NodeDescriptor> nodes;

	if (parse(lines, nodes)) {
		buildModel(nodes);

		if (normalize)
			doNormalize(_root);

		evaluate();
	}
}

void MhaModel::createModelFromString(const char* input, bool normalize) {
	_isModelReady = false;

	if (input == NULL || strlen(input) == 0)
		throw Exception("Не указан файл данных");

	vector<char*> lines;
	istringstream* iss = new istringstream(input);
	const int bufSize = 128;
	char* chBuf = new char[bufSize];

	// Считываем входную строку частями, с разбиением по символу переноса строки,
	// и заполняем массив строк.
	while (iss->getline(chBuf, bufSize)) {
		lines.push_back(chBuf);

		chBuf = new char[bufSize];
	}

	delete chBuf;

	// Формируем дерево.
	createModel(lines, normalize);

	int size = lines.size();

	// Удаляем ненужные более строки.
	for (int i = 0; i < size; i++)
		delete lines.at(i);
}

void MhaModel::createModelFromFile(const char* input, bool normalize) {
	_isModelReady = false;

	if (input == NULL || strlen(input) == 0)
		throw Exception("Не указан файл данных");

	ifstream infile(input);

	if (!infile)
		throw Exception("Не удалось открыть файл данных");

	const int bufSize = 128;
	char* chBuf = new char[bufSize];
	vector<char*> lines;

	// Считываем входной файл построчно. Заполняем массив строк.
	while (infile.getline(chBuf, bufSize)) {
		lines.push_back(chBuf);

		chBuf = new char[bufSize];
	}

	delete chBuf;

	infile.close();
	// Создаём модель.
	createModel(lines, normalize);

	int size = lines.size();

	// Удаляем ненужные более строки.
	for (int i = 0; i < size; i++)
		delete lines.at(i);
}

void MhaModel::doNormalize(Node* node) {
	int size = node->out.size();
	double sum = 0;

	// Суммируем веса всех узлов уровня.
	for (int i = 0; i < size; i++)
		sum += node->out.at(i)->weight;

	// Делим веса всех узлов на полученную сумму. Нормализуем дальше.
	for (int i = 0; i < size; i++) {
		node->out.at(i)->weight /= sum;

		doNormalize(node->out.at(i)->target);
	}
}

void MhaModel::evaluate(Node* node) {
	int size = node->out.size();

	// Если выходящих связей больше нет, то сохраняем результаты.
	if (size == 0) {
		for (int j = 0; j < _results.size(); j++)
			if (_results.at(j)->num == node->num)
				return;

		_results.push_back(node);
	}
	else	// Иначе считаем приоритет как произведение веса связи на приоритет
				// стоящего выше узла.
		for (int i = 0; i < size; i++) {
			Link *lnk = node->out.at(i);

			lnk->target->priority += node->priority * lnk->weight;

			evaluate(lnk->target);
		}
}

bool MhaModel::evaluate() {
	if (!_isModelReady)
		throw Exception("Нет данных для расчёта");

	evaluate(_root);

	return true;
}

vector<Node*> MhaModel::getResults() {
	return _results;
}
