#include <iostream>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class NetworkNode;

/**
 * Класс, представляющий связь-работу. Включает длину (продолжительность),
 * ссылки на связываемые узлы (вершины, состояния) и вспомогательную
 * логическую переменную, используемую для распределения узлов по слоям.
 */
class Link {
public:
	double length;
	NetworkNode* src;
	NetworkNode* dst;
	bool excluded;

	Link(double l, NetworkNode* s, NetworkNode* d) {
		length = l;
		src = s;
		dst = d;
		excluded = false;
	}
};

/**
 * Класс для представления состояния на сетевом графике. Содержит
 * номер вершины, раннее и позднее время, а также резерв.
 */
class NetworkNode {
private:
	vector<Link*>* _in;
	vector<Link*>* _out;
	double _Te;
	double _Tl;
	double _R;
	// Номер вершины. Первоначально используется при чтении из файла
	// и построении сети, а затеи при нумерации узлов.
	int _number;

	void defaultConstructor();
	string nodeListToString(const char* title, const vector<NetworkNode*>* data);
	vector<NetworkNode*>* extractNodes(vector<Link*>* links, bool src);

public:
	NetworkNode();
	NetworkNode(int number);
	~NetworkNode();

	void setNumber(int number);
	void setTEarliest(double v);
	void setTLatest(double v);
	void setReserve(double v);

	int getNumber();
	double getTEarliest();
	double getTLatest();
	double getReserve();

	void addOutgoingNode(NetworkNode* node, double lengthTo);

	const vector<Link*>* getOutgoingLinks() { return _out; }
	const vector<Link*>* getIncomingLinks() { return _in; }

	const vector<NetworkNode*>* getOutgoingNodes() { return extractNodes(_out, false); }
	const vector<NetworkNode*>* getIncomingNodes() { return extractNodes(_in, true); }

	string toString();
	string getDescription();
};

/**
 * Сетевой график. Представляет собой сеть связанных узлов.\
 */
class NetworkPlan {
	/**
	 * Вспомогательный класс для хранения работы, которая
	 * направлена к узлу с номером num и имеет длину len.
	 */
	class NumLen {
	public:
		int num;
		double len;

		NumLen(int num, double len) {
			this->num = num;
			this->len = len;
		}
	};

	/**
	 * Вспомогательный класс для хранения данных из строки описания
	 * узла, считанной из файла.
	 */
	class NodeInfo {
	public:
		NetworkNode* n;
		vector<NumLen> outLinks;

		NodeInfo(NetworkNode* n, vector<NumLen> outLinks) {
			this->n = n;
			this->outLinks = outLinks;
		}

		~NodeInfo() {
			outLinks.clear();
		}
	};

	NetworkNode* _S;
	NetworkNode* _F;
	int _total;
	double _Tcrit;
	vector<vector<NetworkNode*>*>* _layers;
	vector<vector<NetworkNode*>*>* _critPaths;

	bool parse(vector<char*> lines, vector<NodeInfo> &nodes);
	bool buildModel(vector<NodeInfo> nodes);
	void createModel(vector<char*> lines);

	void extractLayers();
	void numberNodes();
	void findCriticalPaths(NetworkNode* cur);

public:
	/**
	 * Общий класс исключений.
	 */
	class Exception {
	protected:
		string _msg;
	public:
		Exception(const char* msg) {
			_msg = msg;
		}

		string getMessage() { return _msg; }
	};

	/**
	 * Класс исключений для сети.
	 */
	class NetworkException : public Exception {
	public:
		NetworkException(const char* msg) : Exception(msg) { };
	};

	/**
	 * Класс исключений для неверно заданных аргументов.
	 */
	class ArgumentException : public Exception {
	public:
		ArgumentException(const char* msg) : Exception(msg) { };
	};

	NetworkPlan(const char* input, bool isFile = true);
	~NetworkPlan();

	void createModelFromFile(const char* input);
	void createModelFromString(const char* input);

	// Рассчитать результаты.
	void calculate();
	// Получить критическое время.
	double getTCritical() { return _Tcrit; }
	// Получить стартовую вершину.
	const NetworkNode* getS() { return _S; }
	// Получить финишную вершину.
	const NetworkNode* getF() { return _F; }
	// Получить критические пути (массив массивов).
	const vector<vector<NetworkNode*>*>* getCriticalPaths() { return _critPaths; }
};
