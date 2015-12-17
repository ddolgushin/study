#include <vector>
#include <string>

using namespace std;

struct Link;

// Структура для хранения сведений об узлах иерархии.
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

// Структура для хранения сведений о связи между узлами иерархии.
struct Link {
	Node* target;
	double weight;

	Link(Node* t, double w) {
		target = t;
		weight = w;
	}
};

/**
 * Модель данных для хранения иерархии.
 */
class MhaModel {
	// Вспомогательная структура, используемая при чтении данных.
	struct LinkDescriptor {
		int targetNum;
		double weight;

		LinkDescriptor(int tn, double w) {
			targetNum = tn;
			weight = w;
		}
	};

	// Структура для хранения сведений об узлах иерархии.
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

	Node *_root; // Указатель на корень дерева.
	vector<Node*> _allNodes; // Список всех узлов дерева.
	vector<Node*> _results; // Список результатов, заполняемый при расчёте.
	bool _isModelReady; // Признак готовновти модели к расчёту. ИСТИНА если считывание произведено успешно.

	// Разбивает строку пробелами (табуляциями):
	// [0 "Target node" 11:5 12:5 13:5 14:5] => [0] [Target node] [11:5] [12:5] [13:5] [14:5].
	vector<string> xsplit(string buf);
	// Производит разбор массива строк и преобразует его в список вспомогательных структур.
	bool parse(vector<char*> lines, vector<NodeDescriptor> &nodes);
	// Строит модель по данным, заранее считанным из файла или строки.
	bool buildModel(vector<NodeDescriptor> nodes);
	// Формирует модель, обеспечивая её построение, нормализацию и расчёт результатов.
	void createModel(vector<char*> lines, bool normalize = true);

	// Производит расчёт (синтез приоритетов). Рекурсивный метод.
	void evaluate(Node* node);
	// Производит расчёт (синтез приоритетов). Предварительно делает проверку готовности модели
	// и вызывает рекурсивный метод для корня дерева.
	bool evaluate();
	// Нормализует дерево. Рекурсивный метод.
	void doNormalize(Node* node);
	void clearModel();

public:
	// Общий класс для исключительных ситуаций.
	class Exception {
	protected:
		string _msg;
	public:
		Exception(const char* msg) {
			_msg = msg;
		}

		string getMessage() { return _msg; }
	};

	// Конструктор. Принимает на входе строку, которая может быть названием
	// файла (isFile = true) или строкой с данными (isFile = false).
	MhaModel(const char* input, bool normalize = true, bool isFile = true);
	~MhaModel();

	// Формирует модель на основе файла.
	void createModelFromFile(const char* input, bool normalize = true);
	// Формирует модель на основе строки.
	void createModelFromString(const char* input, bool normalize = true);
	// Отдаёт результаты в виде коллекции.
	vector<Node*> getResults();
};
