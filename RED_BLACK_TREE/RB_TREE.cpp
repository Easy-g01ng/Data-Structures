#include <iostream>
#include <string>
#include <vector>

constexpr unsigned char RED = 1;
constexpr unsigned char BLACK = 0;

typedef struct User_Data {
	//用户自定义数据类型
	int key;
	//后续可根据需求拓展
} Data;

typedef struct RBtree_Node {
	Data* data;
	RBtree_Node* left;
	RBtree_Node* right;
	RBtree_Node* father;
	unsigned char color;
} RBNode;

class RBtree {

private:
	RBNode* root;
	RBNode* nul;

	RBtree();
	RBtree(const RBtree& rbtree) = delete;
	RBtree& operator=(const RBtree& rbtree) = delete;

	void RBtree_left_rotate(RBNode* node);
	void RBtree_right_rotate(RBNode* node);

	void insert_fixup(RBNode* node);//插入修复函数
	void delete_fixup(RBNode* node);//删除修复函数

	void destroy(RBNode* node);//辅助析构函数

	void Inorder_traversal_By_Recursion(RBNode* node) const;
	void Inorder_traversal_By_Cycle(RBNode* node) const;

public:
	~RBtree();

	template<typename... Args>
	static std::unique_ptr<RBtree> Initialize(Args&&... args);

	void RBtree_insert(const int& key);
	void RBtree_insert(const RBNode* node);

	void RBtree_delete(const int& key);

	void inorder_traversal_from_root() const;
	void inorder_traversal_from_somenode(const RBNode* node) const;
};

RBtree::RBtree() {
	nul = new RBNode;
	nul->color = BLACK;
	nul->data = nullptr;
	nul->father = nul->left = nul->right = nul;
	root = nul;
}

void RBtree::RBtree_left_rotate(RBNode* x) {
	RBNode* y = x->right;
	//修改x节点的右指向以及y节点左子节点（先判断是否存在）的父亲指向
	x->right = y->left;
	if (y->left != nul) {
		y->left->father = x;
	}
	//修改y节点的父亲指向以及原先x节点的父亲节点对孩子节点的指向（或者更新根节点）
	y->father = x->father;
	if (x->father == nul) {
		root = y;
	}
	else if (x == x->father->left) {
		x->father->left = y;
	}
	else {
		x->father->right = y;
	}
	//修改y节点的左指向以及x节点的父亲指向
	y->left = x;
	x->father = y;
}

void RBtree::RBtree_right_rotate(RBNode* y) { //与左旋同理，将x和y以及left和right互换即可
	RBNode* x = y->left;
	y->left = x->right;
	if (x->right != nul) {
		x->right->father = y;
	}
	x->father = y->father;
	if (y->father == nul) {
		root = x;
	}
	else if (y == y->father->right) {
		y->father->right = x;
	}
	else {
		y->father->left = x;
	}
	x->right = y;
	y->father = x;
}

void RBtree::insert_fixup(RBNode* node) {
	while (node->father->color == RED) {
		if (node->father == node->father->father->left) {
			RBNode* Uncle = node->father->father->right;
			if (Uncle->color == RED) {
				node->father->color = BLACK;
				Uncle->color = BLACK;
				Uncle->father->color = RED;
				node = Uncle->father;
			}
			else {
				if (node == node->father->right) {
					node = node->father;
					RBtree_left_rotate(node);
				}
				node->father->color = BLACK;
				node->father->father->color = RED;
				RBtree_right_rotate(node->father->father);
			}
		}
		else {
			RBNode* Uncle = node->father->father->left;
			if (Uncle->color == RED) {
				node->father->color = BLACK;
				Uncle->color = BLACK;
				Uncle->father->color = RED;
				node = Uncle->father;
			}
			else {
				if (node == node->father->left) {
					node = node->father;
					RBtree_right_rotate(node);
				}
				node->father->color = BLACK;
				node->father->father->color = RED;
				RBtree_left_rotate(node->father->father);
			}
		}
	}
	root->color = BLACK;
	return;
}
void RBtree::delete_fixup(RBNode* node) {
	return;
}

void RBtree::destroy(RBNode* node) {
	if (node == nul) {
		return;
	}
	destroy(node->left);
	destroy(node->right);
	delete node->data;
	delete node;
}

void RBtree::Inorder_traversal_By_Recursion(RBNode* node) const {
	//递归出口
	if (node == nul) { return; }
	//左 中 右
	Inorder_traversal_By_Recursion(node->left);
	std::cout << node->data->key << " ";
	Inorder_traversal_By_Recursion(node->right);
}

void RBtree::Inorder_traversal_By_Cycle(RBNode* node) const {
	std::vector<RBNode*> stack_;
	RBNode* current_ptr = node;

	while (current_ptr != nul || !stack_.empty()) {
		while (current_ptr != nul) {
			stack_.emplace_back(current_ptr);
			current_ptr = current_ptr->left;
		}

		//压栈完毕开始访问栈顶节点
		std::cout << stack_.back()->data->key << " ";
		current_ptr = stack_.back()->right;
		stack_.pop_back();
	}
	return;
}

RBtree::~RBtree() {
	destroy(root);
	delete nul;
}

template<typename... Args>
static std::unique_ptr<RBtree> RBtree::Initialize(Args&&... args) {
	auto tree = std::unique_ptr<RBtree>(new RBtree());
	(tree->RBtree_insert(static_cast<int>(args)), ...);
	return tree;
}

void RBtree::RBtree_insert(const int& key) {

	RBNode* y = new RBNode;
	y->data = new Data{ key };
	y->left = y->right = nul;
	y->color = 1;//默认插入的新节点为红色

	RBNode* target = nul;
	RBNode* x = root;

	while (x != nul) {
		target = x;
		if (key < x->data->key) {
			x = x->left;
		}
		else if (key > x->data->key) {
			x = x->right;
		}
		else {
			//如需实现相同key值情况下的业务拓展，再在此处编写代码
			delete y;
			return;
		}
	}
	y->father = target;
	if (target == nul) {
		root = y;
		y->color = 0;//第一个插入的节点作为根节点必须是黑色
	}
	else if (key < target->data->key) {
		target->left = y;
	}
	else {
		target->right = y;
	}

	insert_fixup(y);
	return;
}

void RBtree::RBtree_insert(const RBNode* node) {
	return;
}

void RBtree::RBtree_delete(const int& key) {

	return;
}

void RBtree::inorder_traversal_from_root() const {
	Inorder_traversal_By_Cycle(root);
	return;
}

void RBtree::inorder_traversal_from_somenode(const RBNode* node) const {
	if (node != nullptr && node != nul) {
		Inorder_traversal_By_Cycle(const_cast<RBNode*>(node));
	}
	return;
}

int main() {
	auto rb_tree_ = RBtree::Initialize(6, 4, 5, 3, 0, 8, 9, 7);
	rb_tree_->RBtree_insert(100);
	rb_tree_->inorder_traversal_from_root();
	return 0;
}
