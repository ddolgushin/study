#include <iostream>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class NetworkNode;

//  ласс, представл€ющий св€зь-работу. ¬ключает длину (продолжительность),
// ссылки на св€зываемые узлы (вершины, состо€ни€) и вспомогательную
// логическую переменную, используемую дл€ распределени€ узлов по сло€м.
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

//  ласс дл€ представлени€ состо€ни€ на сетевом графике. —одержит
// номер вершины, раннее и позднее врем€, а также резерв.
class NetworkNode {
private:
	vector<Link*>* _in;
	vector<Link*>* _out;
	double _Te;
	double _Tl;
	double _R;
	// Ќомер вершины. ѕервоначально используетс€ при чтении из файла
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

// —етевой график. ѕредставл€ет собой сеть св€занных узлов.
class NetworkPlan {
	// ¬спомогательный класс дл€ хранени€ работы, котора€
	// направлена к узлу с номером num и имеет длину len.
	class NumLen {
	public:
		int num;
		double len;

		NumLen(int num, double len) {
			this->num = num;
			this->len = len;
		}
	};

	// ¬спомогательный класс дл€ хранени€ данных из строки описани€
	// узла, считанной из файла.
	class NodeInfo {
	public:
		NetworkNode* n;
		vector<NumLen*> outLinks;

		NodeInfo(NetworkNode* n, vector<NumLen*> outLinks) {
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

	void extractLayers();
	void numberNodes();
	void findCriticalPaths(NetworkNode* cur);

public:
	class Exception {
	protected:
		string _msg;
	public:
		Exception(const char* msg) {
			_msg = msg;
		}

		string getMessage() { return _msg; }
	};

	class NetworkException : public Exception {
	public:
		NetworkException(const char* msg) : Exception(msg) { };
	};

	class LoadException : public Exception {
	public:
		LoadException(const char* msg) : Exception(msg) { };
	};

	class ArgumentException : public Exception {
	public:
		ArgumentException(const char* msg) : Exception(msg) { };
	};

	NetworkPlan(const char* inputFile);
	~NetworkPlan();

	void calculate();
	double getTCritical() { return _Tcrit; }
	const NetworkNode* getS() { return _S; }
	const NetworkNode* getF() { return _F; }
	const vector<vector<NetworkNode*>*>* getCriticalPaths() { return _critPaths; }
};
