
/* [ RED-BLACK-TREE CLASS HEADER FILE ]
 * 
 *   This is a very typical implementation of a Red-Black binary tree.
 *   
 *   The functions document the logic inside themselves, however,
 *   an explanation of how red-black trees work as a whole is not provided here.
 *   They are well documented online, you will find much better explanations there.*/

#ifndef REDBLACKTREE_HPP
# define REDBLACKTREE_HPP

# include <EMMA.hpp>

namespace emma
{
	class RedBlackTree
	{
		public:
			class Node // Class used to represent one node of the tree
			{
				public:
					enum Color // Color of the node
					{
						BLACK,
						RED
					};

					Node(std::size_t value) :
					left(NULL), right(NULL), parent(NULL), value(value), color(BLACK) {}

					~Node() {}

					// Note, using getters/setters for these would be excessive.
					Node*		left;
					Node*		right;
					Node*		parent;
					std::size_t	value;
					enum Color	color;
			};

			RedBlackTree();
			~RedBlackTree();

			void	insert_node(Node* new_node);
			void	remove_node(Node* node_to_delete);
			Node*	search_best_fit(const std::size_t size);

		private:
			Node*	m_root;

			// Helper functions used internally to maintain tree structure
			void	rotate_node_left(Node* target);
			void	rotate_node_right(Node* target);
			void	transplant_node(Node* dest_node, Node* src_node);
			void	fix_insert_node_violations(Node* target);
			void	fix_remove_node_violations(Node* target);
			Node*	get_smallest_in_subtree(Node* target);
	};
};

#endif
