
/* [ RED-BLACK-TREE CLASS DEFINITION FILE ]
 *
 * This is a very typical implementation of a Red-Black binary tree.
 * Only difference is that conventional RB-tree variable names like x/y/z
 * are not used due to making the operational logic hard to follow.
 *
 * The functions document the logic inside themselve, however,
 * an explanation of how red-black trees work as a whole is not provided here.
 * They are well documented online, you will find much better explanations there.
 *
 * view 'functions_and_classes_list.txt' for added documentation */

#include <EMMA.hpp>
#include <RedBlackTree.hpp>

// Undefined at the end of this file
#define RED emma::RedBlackTree::Node::RED
#define BLACK emma::RedBlackTree::Node::BLACK

emma::RedBlackTree::RedBlackTree() : m_root(NULL) {}

emma::RedBlackTree::~RedBlackTree() {}

void emma::RedBlackTree::insert_node(Node* new_node)
{/* Params    : Ptr to the node to add to the tree
 *  On success: Adds the node to the tree
 *  On failure: Does nothing
 *  Fails if  : Node is NULL */

	if (new_node == NULL)
		return;

	new_node->left   = NULL; 
	new_node->right  = NULL;
	new_node->color  = RED;

	// 1. Traverse nodes down from root until we reach the bottom
	Node* parent_node  = NULL;
	Node* current_node = this->m_root;
	while (current_node != NULL)
	{
		parent_node = current_node;
		if (new_node->value < current_node->value)
			current_node = current_node->left;
		else
			current_node = current_node->right;
	}

	// 2. Update new node as child of the last node we traversed.
	new_node->parent = parent_node;
	if (parent_node == NULL)
		this->m_root = new_node;
	else if (new_node->value < parent_node->value)
		parent_node->left = new_node;
	else
		parent_node->right = new_node;

	// 3. We may have violated the structure of the tree. Fix it!
	fix_insert_node_violations(new_node);
}


void emma::RedBlackTree::remove_node(Node* target_node)
{/* Params    : Ptr to the node to remove from the tree
 *  On success: Removes the node from the tree. Does not deconstruct it!
 *  On failure: Does nothing
 *  Fails if  : Target node is NULL */

	if (target_node == NULL)
		return;

	enum Node::Color	original_color = target_node->color;
	Node*	temp_node = target_node;
	Node*	replacing_node = NULL;

	// We want to replace the original node with the child node.
	// If we have 0 or 1 children, this is easy - let's check for that.
	if (target_node->left == NULL)
	{
		replacing_node = target_node->right;
		transplant_node(target_node, target_node->right);
	}
	else if (target_node->right == NULL)
	{
		replacing_node = target_node->left;
		transplant_node(target_node, target_node->left);
	}
	else // Bummer, it has both children...
	{
		// Get the smallest value node from our right child's subtree
		temp_node = get_smallest_in_subtree(target_node->right);
		original_color = temp_node->color;

		// If the smallest node's parent is the target, set the replacing node's
		// parent as the smallest node.
		replacing_node = temp_node->right;
		if (temp_node->parent == target_node)
			replacing_node->parent = temp_node;
		else
		{
			transplant_node(temp_node, temp_node->right);
			temp_node->right = target_node->right;
			if (temp_node->right != NULL)
				temp_node->right->parent = temp_node;
		}
		transplant_node(target_node, temp_node);
		temp_node->left = target_node->left;
		if (temp_node->left != NULL)
			temp_node->left->parent = temp_node;
		temp_node->color = target_node->color;
	}

	// Violations may have occured if the original node was black. Fix it!
	if (original_color == BLACK)
		fix_remove_node_violations(replacing_node);
}


emma::RedBlackTree::Node* emma::RedBlackTree::search_best_fit(std::size_t target_size)
{/* Params    : Size to find the closest matching node of
 *  On success: Finds a node with the closest matching size from the whole tree
 *  On failure: Returns NULL
 *  Fails if  : Tree is empty */
	
	Node*	parent_node = NULL;
	Node*	current_node = m_root;

	// Traverse tree downwards
	while (current_node != NULL)
	{
		parent_node = current_node;
		if (target_size == current_node->value)
			break;
		else if (target_size < current_node->value)
			current_node = current_node->left;
		else
			current_node = current_node->right;
	}

	// Traverse back upwards until we hit a value >= target
	while (parent_node != NULL && target_size > parent_node->value)
		parent_node = parent_node->parent;
	return (parent_node);
}


emma::RedBlackTree::Node* emma::RedBlackTree::get_smallest_in_subtree(Node* target)
{/* Params    : Ptr to the node we want to search the subtree of
 *  On success: Returns the node with the smallest value in the subtree
 *  On failure: Returns NULL
 *  Fails if  : Target node is NULL */

	if (target == NULL)
		return NULL; 

	while (target->left != NULL)
		target = target->left;

	return target;
}

void emma::RedBlackTree::fix_insert_node_violations(Node* current_node)
{/* Params    : Ptr to a node we just inserted.
 *  On success: Checks if the insertion violated rules, and fixes them.
 *  On failure: Does nothing.
 *  Fails if  : Node is NULL. */

	if (current_node == NULL)
		return;

	Node* parent_node		= NULL;
	Node* grandparent_node	= NULL;

	// Keeps traversing upwards until there are no more violations to fix
	while (current_node != this->m_root && current_node->color == RED 
			&& current_node->parent->color == RED)
	{
		parent_node = current_node->parent;
		grandparent_node = parent_node->parent;

		if (parent_node == grandparent_node->left)
		{
			Node* uncle_node = grandparent_node->right;
			if (uncle_node != NULL && uncle_node->color == RED)
			{
				grandparent_node->color = RED;
				parent_node->color = BLACK;
				uncle_node->color = BLACK;
				current_node = grandparent_node;
			}
			else
			{
				if (current_node == parent_node->right)
				{
					rotate_node_left(parent_node);
					current_node = parent_node;
					parent_node = current_node->parent;
				}
				rotate_node_right(grandparent_node);
				std::swap(parent_node->color, grandparent_node->color);
				current_node = parent_node; //Move up in the tree
			}
		}
		else
		{
			Node* uncle_node = grandparent_node->left;
			if (uncle_node != NULL && uncle_node->color == RED)
			{
				grandparent_node->color = RED;
				parent_node->color = BLACK;
				uncle_node->color = BLACK;
				current_node = grandparent_node;
			}
			else
			{
				if (current_node == parent_node->left)
				{
					rotate_node_right(parent_node);
					current_node = parent_node;
					parent_node = current_node->parent;
				}
				rotate_node_left(grandparent_node);
				std::swap(parent_node->color, grandparent_node->color);
				current_node = parent_node;
			}
		}
	}
	this->m_root->color = BLACK;
}


void emma::RedBlackTree::fix_remove_node_violations(Node* current_node)
{/* Params    : Ptr to the node that replaced the one which was removed
 *  On success: Fixes rule violations the remove operation may have caused
 *  On failure: Does nothing
 *  Fails if  : Target node is NULL */

	if (current_node == NULL)
		return;

	// Traverse the tree upwards starting from the node.
	// There are no more violations if the current node is red or root
	while (current_node != this->m_root && current_node->color == BLACK)
	{
		if (current_node == current_node->parent->left) // We are left child
		{
			Node* sibling_node = current_node->parent->right;

			// Fixes red siblings
			if (sibling_node->color == RED)
			{
				sibling_node->color = BLACK;
				current_node->parent->color = RED;
				rotate_node_left(current_node->parent);
				sibling_node = current_node->parent->right;
			}
			// Fixes siblings with 2 black children
			if ((sibling_node->left == NULL || sibling_node->left->color == BLACK) 
					&& (sibling_node->right == NULL || sibling_node->right->color == BLACK))
			{
				sibling_node->color = RED;
				current_node = current_node->parent; // Move up in the tree
			}
			else // The sibling must have 1 or 2 red children
			{
				// Fixes sibling with a left red child & black right child
				if (sibling_node->right == NULL || sibling_node->right->color == BLACK)
				{
					if (sibling_node->left != NULL)
						sibling_node->left->color = BLACK;
					sibling_node->color = RED;
					rotate_node_right(sibling_node);
					sibling_node = current_node->parent->right;
				}
				sibling_node->color = current_node->parent->color;
				sibling_node->parent->color = BLACK;
				if (sibling_node->right != NULL)
					sibling_node->right->color = BLACK;
				rotate_node_right(current_node->parent);
				current_node = this->m_root;
			}
		}
		else // We are right child
		{
			Node* sibling_node = current_node->parent->left;

			// Fixes red siblings
			if (sibling_node->color == RED)
			{
				sibling_node->color = BLACK;
				current_node->parent->color = RED;
				rotate_node_right(current_node->parent);
				sibling_node = current_node->parent->left;
			}
			// Fixes siblings with 2 black children
			if ((sibling_node->left == NULL || sibling_node->left->color == BLACK)
					&& (sibling_node->right == NULL || sibling_node->right->color == BLACK))
			{
				sibling_node->color = RED;
				current_node = current_node->parent;
			}
			else
			{
				// Fixes sibling with a left black child & red right child
				if (sibling_node->left == NULL || sibling_node->left->color == BLACK)
				{
					if (sibling_node->right != NULL)
						sibling_node->right->color = BLACK;
					sibling_node->color = RED;
					rotate_node_left(sibling_node);
					sibling_node = current_node->parent->left;
				}
				sibling_node->color = current_node->parent->color;
				current_node->parent->color = BLACK;
				if (sibling_node->left != NULL)
					sibling_node->left->color = BLACK;
				rotate_node_right(current_node->parent);
				current_node = this->m_root;
			}
		}
	}
	current_node->color = BLACK;
}


void emma::RedBlackTree::transplant_node(Node* dest_node, Node* src_node)
{/* Params    : Destination node & Source node
 *  On success: Replaces destination node with the source node
 *  On failure: Does nothing
 *  Fails if  : Destination is null*/

	if (dest_node == NULL)
		return;

	if (dest_node->parent == NULL) // We are root
		this->m_root = src_node;
	else if (dest_node == dest_node->parent->left) // We are left child
		dest_node->parent->left = src_node;
	else // We are right child
		dest_node->parent->right = src_node;

	if (src_node != NULL)
		src_node->parent = dest_node->parent;
}


void emma::RedBlackTree::rotate_node_left(Node* target_node)
{/* Params    : Ptr to the node to rotate
 *  On success: Rotates the node of the tree to the left, returns nothing.
 *  On failure: Does nothing
 *  Fails if  : Target is NULL */

	if (target_node == NULL)
		return;

	Node*	right_child = target_node->right;
	
	// 1. Set target_node's right child ptr as the current right child's left 
	target_node->right = right_child->left;
	if (target_node->right != NULL) 
		target_node->right->parent = target_node;

	// 2. Set parent of the old right child to the parent of the target_node
	right_child->parent = target_node->parent;

	// 3. Update the parent of the target_node node
	if (target_node->parent == NULL)
		this->m_root = right_child;
	else if (target_node->parent->left == target_node)
		target_node->parent->left = right_child;
	else
		target_node->parent->right = right_child;

	// 4. Set target_node to be a child of the old right child.
	right_child->left = target_node;
	target_node->parent = right_child;
}


void emma::RedBlackTree::rotate_node_right(Node* target_node)
{/* Params    : Ptr to the node to rotate
 *  On success: Rotates the node of the tree to the right, returns nothing.
 *  On failure: Does nothing
 *  Fails if  : Target is NULL */

	if (target_node == NULL)
		return;

	Node*	left_child = target_node->left;
	
	// 1. Set target_node's left child ptr as the current left child's right
	target_node->left = left_child->right;
	if (target_node->left != NULL) 
		target_node->left->parent = target_node;

	// 2. Set parent of the old left child to the parent of the target_node
	left_child->parent = target_node->parent;

	// 3. Update the parent of the target_node node
	if (target_node->parent == NULL)
		this->m_root = left_child;
	else if (target_node->parent->left == target_node)
		target_node->parent->left = left_child;
	else
		target_node->parent->right = left_child;

	// 4. Set target_node to be a child of the old left child.
	left_child->right = target_node;
	target_node->parent = left_child;
}

#undef BLACK
#undef RED
