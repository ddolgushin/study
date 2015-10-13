#include <vector>
#include <string>

using namespace std;

struct Link;

struct Node {
	int num;
	vector<Link*> out;
	double priority;
	string name;

	Node(int n = -1, string nm = "", double p = 0.0) {
		num = n;
		name = nm;
		priority = p;
	}

	~Node() {
		for (int j = 0; j < out.size(); j++)
			delete out.at(j);
	}
};

struct Link {
	Node* target;
	double weight;

	Link(Node* t, double w) {
		target = t;
		weight = w;
	}
};

struct LinkDescriptor {
	int targetNum;
	double weight;

	LinkDescriptor(int tn, double w) {
		targetNum = tn;
		weight = w;
	}
};

struct NodeDescriptor {
	int num;
	string name;
	vector<LinkDescriptor> links;

	NodeDescriptor(int n, string nm, vector<LinkDescriptor> l)
	{
		num = n;
		name = nm;
		links = l;
	}
};

class MhaModel {
	Node *_root;
	vector<Node*> _allNodes;
	vector<Node*> _results;
	bool _isModelReady;

	void evaluate(Node* node);
	bool buildModel(vector<NodeDescriptor> nodes);
	bool parse(vector<char*> lines, vector<NodeDescriptor> &nodes);
	void doNormalize(Node* node);
	bool evaluate();
	void createModel(vector<char*> lines, bool normalize = true);

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

	MhaModel(const char* input, bool normalize = true, bool isFile = true);
	~MhaModel();

	void createModelFromFile(const char* fileName, bool normalize = true);
	void createModelFromString(const char* data, bool normalize = true);
	vector<Node*> getResults();
};
