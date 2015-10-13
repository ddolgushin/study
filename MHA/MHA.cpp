#include <iterator>
#include <sstream>
#include <fstream>

#include "MHA.h"

using namespace std;

void MhaModel::doNormalize(Node* node) {
	int size = node->out.size();
	double sum = 0;

	for (int i = 0; i < size; i++)
		sum += node->out.at(i)->weight;

	for (int i = 0; i < size; i++) {
		node->out.at(i)->weight /= sum;

		doNormalize(node->out.at(i)->target);
	}
}

void MhaModel::evaluate(Node* node) {
	int size = node->out.size();

	if (size == 0) {
		for (int j = 0; j < _results.size(); j++)
			if (_results.at(j)->num == node->num)
				return;

		_results.push_back(node);
	}
	else
		for (int i = 0; i < size; i++) {
			Link *lnk = node->out.at(i);

			lnk->target->priority += node->priority * lnk->weight;

			evaluate(lnk->target);
		}
}

bool MhaModel::buildModel(vector<NodeDescriptor> nodes) {
	_allNodes.clear();

	int size = nodes.size();

	// Создаём узлы по количеству считанных описаний.
	for (int i = 0; i < size; i++)
		_allNodes.push_back(new Node(nodes.at(i).num, nodes.at(i).name, i == 0 ? 1 : 0));

	// Установка указателя на начальную вершину.
	_root = _allNodes.at(0);

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

bool MhaModel::parse(vector<char*> lines, vector<NodeDescriptor> &nodes) {
	vector<LinkDescriptor> linkBuf;
	int size = lines.size();
	const int bufSize = 128;

	for (int i = 0; i < size; i++) {
		char* chBuf = lines.at(i);
		vector<string> tokens;
		istringstream* iss = new istringstream(chBuf);

		copy(istream_iterator<string>(*iss),
			istream_iterator<string>(),
			back_inserter<vector<string>>(tokens));
		linkBuf.clear();

		delete iss;

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
				iss = new istringstream(tokens.at(i));
				int j = 0;
				string* strBuf = NULL;

				while (iss->getline(chBuf, bufSize, ':')) {
					strBuf = new string(chBuf);

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

				delete iss;

				linkBuf.push_back(LinkDescriptor(tmpNum2, tmpWeight));
			}
		}

		nodes.push_back(NodeDescriptor(tmpNum, tmpName, linkBuf));
		tokens.clear();
	}

	return true;
}

MhaModel::MhaModel(const char* input, bool normalize, bool isFile) {
	if (isFile)
		createModelFromFile(input, normalize);
	else
		createModelFromString(input, normalize);
}

MhaModel::~MhaModel() {
	for (int i = 0; i < _allNodes.size(); i++)
		delete _allNodes.at(i);
}

void MhaModel::createModel(vector<char*> lines, bool normalize) {
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

	while (iss->getline(chBuf, bufSize)) {
		lines.push_back(chBuf);

		chBuf = new char[bufSize];
	}

	delete chBuf;

	createModel(lines, normalize);

	int size = lines.size();

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

	while (infile.getline(chBuf, bufSize)) {
		lines.push_back(chBuf);

		chBuf = new char[bufSize];
	}

	delete chBuf;

	infile.close();
	createModel(lines, normalize);

	int size = lines.size();

	for (int i = 0; i < size; i++)
		delete lines.at(i);
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
