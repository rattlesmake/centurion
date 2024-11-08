/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once


#include <cmath>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>


constexpr double LOWEST_DOUBLE = std::numeric_limits<double>::lowest();


/**
 * This class implements a Fibonacci heap data structure. 
 * The amortized running time of most of these methods is O(1), making it a very fast data structure. 
 *
 * Note that this implementation is not thread safe. If multiplevector
 * threads access a set concurrently, and at least one of the threads modifies
 * the set, it must be synchronized externally. This is typically accomplished
 * by synchronizing on some object that naturally encapsulates the set.
 */
template <typename T>
class FibonacciHeap
{
public:
    template <typename T>
    class FibonacciHeapNode
    {
        friend class FibonacciHeap;
    private:
        T current_node_data{};

        std::shared_ptr<FibonacciHeapNode<T>> first_child_node;
        std::shared_ptr<FibonacciHeapNode<T>> parent_node;
        std::shared_ptr<FibonacciHeapNode<T>> left_sibling_node;
        std::shared_ptr<FibonacciHeapNode<T>> right_sibling_node;

        /*
         * True if this node has had a child removed since this node was added to its parent
         */
        bool mark = false;

        /*
         * Primary key value (i.e. priority) associated with data object. It's minimum value can be LOWEST_DOUBLE + 1
         */
        double primary_key = 0.0;

        /*
         * A secondary key value (i.e. priority) associated with data object used used for tie-breaking
         */
        double secondary_key = 0.0;

        // key = primary key U secondary_key

        /*
         * Number of children of this node(does not count grandchildren).
         */
        uint32_t degree = 0;

        #pragma region Private constructor:
        FibonacciHeapNode(T&& data, const double _primary_key, const double _secondary_key) :
            current_node_data(std::move(data)), primary_key(_primary_key), secondary_key(_secondary_key), mark(false)
        {
        }
        #pragma endregion


        #pragma region Private static members:
        [[nodiscard]] static long getTmpKey(const double key) noexcept
        {
            static const long FIBONACCI_HEAP_NODE_BIG_ONE = 100000;
            return static_cast<long>(key * FIBONACCI_HEAP_NODE_BIG_ONE + 0.5);
        }
        #pragma endregion
    public:
        #pragma region Operators:
        FibonacciHeapNode(const FibonacciHeapNode& other) = delete;
        FibonacciHeapNode& operator=(const FibonacciHeapNode& other) = delete;


        /*
         * This operator< returns true if 'this' node has lower priority than 'other' node.
         */
        [[nodiscard]] bool operator<(const FibonacciHeapNode& other) const noexcept
        {
            long tmp_key_this = FibonacciHeapNode<T>::getTmpKey(this->primary_key);
            long tmp_key_other = FibonacciHeapNode<T>::getTmpKey(other.primary_key);
            if (tmp_key_this < tmp_key_other)
                return true;

            // Tie-break in favour of node with higher secondary_key value
            if (tmp_key_this == tmp_key_other)
            {
                tmp_key_this = FibonacciHeapNode::getTmpKey(this->secondary_key);
                tmp_key_other = FibonacciHeapNode::getTmpKey(other.secondary_key);
                if (tmp_key_this > tmp_key_other)
                    return true;
            }
            return false;
        }
        #pragma endregion


        [[nodiscard]] double getPrimaryKey(void) const noexcept
        {
            return this->primary_key;
        }


        [[nodiscard]] double getSecondaryKey(void) const noexcept
        {
            return this->secondary_key;
        }


        [[nodiscard]] T getData(void) const noexcept
        {
            return this->current_node_data;
        }
    };
private:
	/*
     * A pointer to the min node in the Fibonacci heap
     */
	std::shared_ptr<FibonacciHeapNode<T>> min_node;

	/*
     * The number of nodes in the Fibonacci heap
     */
	uint32_t nodes_number = 0;

    /*
     * Allow to set LOWEST_DOUBLE to a Fibonacci heap node.
     * This value should be set to true when decreasePrimaryKey is called internally.
     */
    bool allow_minus_infinity = false;

    /*
     * Links two Fibonacci heap nodes in parent child relationship 
     * Running time: O(1)
     * @param y node to become children of x
     * @param x node to become parent of y
     */
    void link(const std::shared_ptr<FibonacciHeapNode<T>>& y, const std::shared_ptr<FibonacciHeapNode<T>>& x)
    {
        if (!y || !x)
            throw std::invalid_argument("Empty Fibonacci Heap Node");

        // Remove y from root list of Fibonacci heap
        y->left_sibling_node->right_sibling_node = y->right_sibling_node;
        y->right_sibling_node->left_sibling_node = y->left_sibling_node;
        if (x->right_sibling_node == x)
            this->min_node = x;
        y->left_sibling_node = y;
        y->right_sibling_node = y;

        // Make y a child of x
        y->parent_node = x;
        if (!x->first_child_node) //if x hasn't got a child node
            x->first_child_node = y;
        y->right_sibling_node = x->first_child_node;
        y->left_sibling_node = x->first_child_node->left_sibling_node;
        x->first_child_node->left_sibling_node->right_sibling_node = y;
        x->first_child_node->left_sibling_node = y;
        if (*y < *x->first_child_node)
            x->first_child_node = y;
        x->degree += 1;
    }


    /**
     * The reverse of the link operation: removes x from the child list of y.
     * This method assumes that min is non-null.
     * Running time: O(1)
     * @param x child of y to be removed from y's child list
     * @param y parent of x about to lose a child
     */
    void cut(const std::shared_ptr<FibonacciHeapNode<T>>& x, const std::shared_ptr<FibonacciHeapNode<T>>& y)
    {
        // Remove x from childlist of y and decrement degree[y]
        x->left_sibling_node->right_sibling_node = x->right_sibling_node;
        x->right_sibling_node->left_sibling_node = x->left_sibling_node;
        y->degree -= 1;

        // Reset y->child if necessary
        if (y->first_child_node == x) 
            y->first_child_node = x->right_sibling_node;

        if (y->degree == 0) 
            y->first_child_node = std::shared_ptr<FibonacciHeapNode<T>>();

        // Add x to root list of Fibonacci heap
        x->left_sibling_node = this->min_node;
        x->right_sibling_node = this->min_node->right_sibling_node;
		this->min_node->right_sibling_node = x;
        x->right_sibling_node->left_sibling_node = x;

        // Set parent[x] to nullptr
        x->parent_node = std::shared_ptr<FibonacciHeapNode<T>>();

        // Set mark[x] to false
        x->mark = false;
    }


    /*
     * Performs a cascading cut operation. This cuts y from its parent and then does the same for its parent, and so on up the tree.
     * Running time: O(log n); O(1) excluding the recursion
     * @param y node to perform cascading cut on
     */
    void cascadingCut(const std::shared_ptr<FibonacciHeapNode<T>>& y)
    {
        // Get parent node of y
        std::shared_ptr<FibonacciHeapNode<T>> z = y->parent_node;

        // If y has a parent node
        if (!z)
        {
            // If y is unmarked, mark it
            if (y->mark == false) 
            {
                y->mark = true;
            }
            else
            {
                // It's marked, cut it from parent
                this->cut(y, z);

                // Cut its parent as well
                this->cascadingCut(z);
            }
        }
    }


    /*
     * Consolidates a Fibonacci heap
     */
    void consolidate(void)
    {
        static const double ONE_OVER_LOG_PHI = 1.0 / log((1.0 + sqrt(5.0)) / 2.0);

        const size_t degree_array_size = static_cast<size_t>(floor(log(this->nodes_number) * ONE_OVER_LOG_PHI)) + 1;
        std::vector<std::shared_ptr<FibonacciHeapNode<T>>> degree_array(degree_array_size, std::shared_ptr<FibonacciHeapNode<T>>());

        // Find the number of root nodes
        uint32_t num_roots = 0;
        std::shared_ptr<FibonacciHeapNode<T>> x = this->min_node;
        if (x)
        {
            do
            {
                num_roots += 1;
                x = x->right_sibling_node;
            }   while (x != this->min_node);
        }

        // Loop to execute for each node in the root list
        while (num_roots >= 1)
        {
            // Access this node's degree...
            uint32_t degree = x->degree;
            std::shared_ptr<FibonacciHeapNode<T>> next = x->right_sibling_node;
            // ... and see if there's another of the same degree
            while (true)
            {
                std::shared_ptr<FibonacciHeapNode<T>> y = degree_array.at(degree);
                if (!y)
                    break;  // not found

                // found, so make one of the nodes a child of the other
                if (*y < *x)
                    std::swap(x, y);

                // Fibonacci heap node y disappears from root list
                this->link(y, x);

                // This degree has been handled, go to next one.
                degree_array.at(degree) = std::shared_ptr<FibonacciHeapNode<T>>();
                degree++;
            }

            // Save this node for later when we might encounter another of the same degree
            degree_array.at(degree) = x; 

            // Move forward through list
            x = next;
            num_roots -= 1;
        }

        // Set min node to nullptr (effectively losing the root list) and reconstruct the root list from the array entries in degree_array
        this->min_node = std::shared_ptr<FibonacciHeapNode<T>>();
        for (size_t i = 0; i < degree_array_size; i++)
        {
            std::shared_ptr<FibonacciHeapNode<T>> y = degree_array.at(i);
            if (!y)
                continue;  // not found

            // Found, so add it to the root list
            if (this->min_node)
            {
                // Firstly, remove node from root list
                y->left_sibling_node->right_sibling_node = y->right_sibling_node;
                y->right_sibling_node->left_sibling_node = y->left_sibling_node;

                // Then add to root list, again
                y->left_sibling_node = this->min_node;
                y->right_sibling_node = this->min_node->right_sibling_node;
                this->min_node->right_sibling_node = y;
                y->right_sibling_node->left_sibling_node = y;

                // Finally check if y is a new min.
                if (*y < *this->min_node) 
                {
                    this->min_node = y;
                }
            }
            else
            {
                this->min_node = y;
            }
        }
    }


    /*
     * Search recursively a node having a given primary key and given secondary key.
     */
    static void searchNode(const double par_primary_key, const std::optional<double>& par_secondary_key, std::shared_ptr<FibonacciHeapNode<T>>& found_node, const std::shared_ptr<FibonacciHeapNode<T>>& node)
    {
        if (found_node || !node)
            return;
        else 
        {
            std::shared_ptr<FibonacciHeapNode<T>> temp = node;
            do 
            {
                // Compare keys
                const bool same_primary_keys = temp->primary_key == par_primary_key;
                const bool same_secondary_keys = par_secondary_key.has_value() == false || temp->secondary_key == par_secondary_key.value();

                if (same_primary_keys && same_secondary_keys)
                    found_node = temp;
                else
                {
                    // Not found, search again
                    const std::shared_ptr<FibonacciHeapNode<T>> k = temp->first_child_node;
                    FibonacciHeap<T>::searchNode(par_primary_key, par_secondary_key, found_node, k);
                    temp = temp->right_sibling_node;
                }
            }   while (temp != node && !found_node);
        }
    }
public:
	#pragma region Static methods
	[[nodiscard]] static std::shared_ptr<FibonacciHeapNode<T>> createFibonacciHeapNode(T data, const double primary_key, const double secondary_key)
	{
		std::shared_ptr<FibonacciHeapNode<T>> fhn{ new FibonacciHeapNode<T>(std::move(data), primary_key, secondary_key) };
		fhn->left_sibling_node = fhn;
		fhn->right_sibling_node = fhn;
		return fhn;
	}


    /**
     * Joins two Fibonacci heaps into a new one. 
     * No Fibonacci heap consolidation is performed at this time. The two root lists are simply joined together.
     * Running time: O(1)
     * @param fbh1 first Fibonacci heap
     * @param fbh2 second Fibonacci heap
     * @return new Fibonacci heap containing fbh1 and fbh2
     * @exception invalid_argument Thrown if at least one between fbh1 and fbh2 are invalid pointers.
     * @exception logic_error Thrown if fbh1 and fbh2 point to the same Fibonacci heap.
     */
    [[nodiscard]] static std::shared_ptr<FibonacciHeap<T>> merge(const std::shared_ptr<FibonacciHeap<T>>& fbh1, const std::shared_ptr<FibonacciHeap<T>>& fbh2)
    {
        // Check if two Fibonacci heap pointers are both valid
        if (!fbh1 || !fbh2)
            throw std::invalid_argument("Empty Fibonacci heap");
        if (fbh1 == fbh2)
            throw std::logic_error("Cannot merge a Fibonacci heap with itself");

        std::shared_ptr<FibonacciHeap<T>> fbh_merged{ new FibonacciHeap<T>() };
        fbh_merged->min_node = fbh1->min_node;
        if (fbh_merged->min_node)
        {
            // Concatenate the roots of both the heaps and update min by selecting a minimum key from the new root lists
            if (fbh2->min_node)
            {
                fbh_merged->min_node->right_sibling_node->left_sibling_node = fbh2->min_node->left_sibling_node;
                fbh2->min_node->left_sibling_node->right_sibling_node = fbh_merged->min_node->right_sibling_node;
                fbh_merged->min_node->right_sibling_node = fbh2->min_node;
                fbh2->min_node->left_sibling_node = fbh_merged->min_node;

                if (*fbh2->min_node < *fbh1->min_node)
                    fbh_merged->min_node = fbh2->min_node;
            }
            else 
            {
                fbh_merged->min_node = fbh2->min_node;
            }
            fbh_merged->nodes_number = fbh1->nodes_number + fbh2->nodes_number;
        }

        return fbh_merged;
    }
	#pragma endregion


    /*
     * Inserts a new element in the Fibonacci heap
     * Running time: O(1)
     * @param node_data value to insert
     * @param primary_key key value (i.e. priority) associated with data object. It's minimum value can be LOWEST_DOUBLE + 1
     * @param secondary_key a secondary key value (i.e. priority) associated with data object used used for tie-breaking. Default value is lowest_double
     * @return the inserted Fibonacci heap node
     * @exception invalid_argument Thrown if primary_key is equal to LOWEST_DOUBLE value.
     */
    std::shared_ptr<FibonacciHeapNode<T>> insert(T node_data, const double primary_key, const double secondary_key = LOWEST_DOUBLE)
    {
        if (primary_key == LOWEST_DOUBLE)
            throw std::invalid_argument("primary_key must not be equal to lowest double value (" + std::to_string(LOWEST_DOUBLE) + ")");

        // Create a new node for the element
        auto new_node = FibonacciHeap::createFibonacciHeapNode(std::move(node_data), primary_key, secondary_key);

        if (this->min_node)  // Check if the Fibonacci heap is empty
        {
            // If here, Fibonacci heap is NOT empty. So insert the node into the root list and update min_node
            new_node->left_sibling_node = this->min_node;
            new_node->right_sibling_node = this->min_node->right_sibling_node;
            this->min_node->right_sibling_node = new_node;
            new_node->right_sibling_node->left_sibling_node = new_node;

            if (*new_node < *this->min_node)
                this->min_node = new_node;
        }
        else
        {
            // If here, the Fibonacci heap is empty, so set the new node as a root nodeand mark it as min_node.
            this->min_node = new_node;
        }

        // Increment number of nodes
        this->nodes_number += 1;

        return new_node;
    }


    /*
     * Returns the smallest element in the Fibonacci heap, namely the nide with the minimum value.
     * Running time: O(1) actual
     * @return Fibonacci heap node with the smallest value
     */
    [[nodiscard]] std::shared_ptr<FibonacciHeapNode<T>> getMin(void) const noexcept
    {
        return this->min_node;
    }


    /*
     * Removes the smallest element from the Fibonacci heap.
     * This will cause the trees in the Fibonacci heap to be consolidated, if necessary.
     * Running time: O(log n) amortized
     */
    void deleteMin(void) noexcept
    {
        // Extract min without return it
        auto min = this->extractMin();
    }


    /*
     * Removes the smallest element from the Fibonacci heap. 
     * This will cause the trees in the Fibonacci heap to be consolidated, if necessary.
     * Running time: O(log n) amortized
     * @return node with the smallest value
     */
    [[nodiscard]] std::shared_ptr<FibonacciHeapNode<T>> extractMin(void) noexcept
    {
        std::shared_ptr<FibonacciHeapNode<T>> z = this->min_node;
        if (z)
        {
            uint32_t number_of_kids = min_node->degree;
            std::shared_ptr<FibonacciHeapNode<T>> x = z->first_child_node;
            std::shared_ptr<FibonacciHeapNode<T>> tmp_right;

            // Loop to execute for each child of the min node
            while (number_of_kids >= 1)
            {
                tmp_right = x->right_sibling_node;

                // Remove x from the child list
                x->left_sibling_node->right_sibling_node = x->right_sibling_node;
                x->right_sibling_node->left_sibling_node = x->left_sibling_node;

                // Add x to root list of Fibonacci heap
                x->left_sibling_node = this->min_node;
                x->right_sibling_node = this->min_node->right_sibling_node;
                this->min_node->right_sibling_node = x;
                x->right_sibling_node->left_sibling_node = x;

                // set the parent of x to nullptr
                x->parent_node = std::shared_ptr<FibonacciHeapNode<T>>();
                x = tmp_right;
                number_of_kids -= 1;
            }

            // Remove z from the root list of Fibonacci heap
            z->left_sibling_node->right_sibling_node = z->right_sibling_node;
            z->right_sibling_node->left_sibling_node = z->left_sibling_node;

            if (z == z->right_sibling_node)
            {
                this->min_node = std::shared_ptr<FibonacciHeapNode<T>>();
            }
            else
            {
                this->min_node = z->right_sibling_node;
                this->consolidate();
            }
        }
        return z;
    }


    /**
     * Find a node having a given primary key value and, optionally, a given secondary key value.
     * Running time: O(n)
     * @param par_primary_key the value of the primary key to be found
     * @param par_secondary_key the value of the secondary key to be found. This param is optional. If missing, the search is based only on the primary key
     * @return the first node having the wantd key. Empty if not found
     * @exception invalid_argument Thrown if par_primary_key is equal to LOWEST_DOUBLE.
     */
    [[nodiscard]] std::shared_ptr<FibonacciHeapNode<T>> find(const double par_primary_key, std::optional<double> par_secondary_key = std::optional<double>())
    {
        if (par_primary_key == LOWEST_DOUBLE)
            throw std::invalid_argument("primary_key must not be equal to lowest double value (" + std::to_string(LOWEST_DOUBLE) + ")");

        std::shared_ptr<FibonacciHeapNode<T>> found_node;
        this->searchNode(par_primary_key, par_secondary_key, found_node, this->min_node);
        return found_node;
    }


    /*
     * Tests if the Fibonacci heap is empty or not.
     * Running time: O(1)
     * @return true if the Fibonacci heap is empty, false otherwise
     */
    [[nodiscard]] bool isEmpty(void) const noexcept
    {
        return (!this->min_node);
    }


    /*
     * Get the number of nodes of the Fibonacci heap
     * Running time: O(1)
     * @return the number of nodes of the Fibonacci heap
     */
    [[nodiscard]] uint32_t size(void) const noexcept
    {
        return this->nodes_number;
    }


    /*
     * Removes all elements from this heap.
     * Running time: O(1)
     */
    void clear(void) noexcept
    {
        this->min_node = std::shared_ptr<FibonacciHeapNode<T>>();
        this->nodes_number = 0;
    }


    /*
     * Decreases the primary key value for a Fibonacci heap node, given the new value to take on.
     * The structure of the heap may be changed and will not be consolidated.
     * Running time: O(1) amortized
     * WARNING: attention, if the node does not belong to the Fibonacci heap it is very probably that it will be corrupted after this operation
     * @param x node to decrease the primary key of
     * @param new_primary_key new key value for node x
     * @exception invalid_argument Thrown if node x is an invalid pointer or if new_primary_key is equal to LOWEST_DOUBLE value or if new_primary_key is larger than x.primary_key value.
     */
    void decreasePrimaryKey(const std::shared_ptr<FibonacciHeapNode<T>>& x, const double new_primary_key)
    {
        // Check if Fibonacci heap node pointer is valid
        if (!x)
            throw std::invalid_argument("Empty Fibonacci heap node");

        if (this->allow_minus_infinity == false && new_primary_key == LOWEST_DOUBLE)
            throw std::invalid_argument("primary_key must not be equal to lowest double value (" + std::to_string(LOWEST_DOUBLE) + ")");

        // Compute temporary keys and checks values
        const long tmp_k = FibonacciHeapNode<T>::getTmpKey(new_primary_key); 
        const long tmp_x = FibonacciHeapNode<T>::getTmpKey(x->primary_key);
        if (tmp_k > tmp_x)
            throw std::invalid_argument("decreasePrimaryKey() got larger primary key value");
        if (tmp_k == tmp_x)
            return;  // Values are equals so is useless to continue since primary key will not change

        // Change node primary key value to the new value
		x->primary_key = new_primary_key;

        // Try to get the parent of the node x.
        std::shared_ptr<FibonacciHeapNode<T>> y{ x->parent_node };
        if (y && (*x) < (*y))
        {
           // If x has a parent and the key of parent is greater than x then call cut and cascading subsequently
           this->cut(x, y);
           this->cascadingCut(y);
        }

        // if the key of x is smaller than the key of min node, then mark x as min node
        if ((*x) < (*this->min_node))
        {
            this->min_node = x;
        }
    }


    /*
     * Decreases the primary key value for a Fibonacci heap node, given the new value to take on.
     * It needs firstly to found the node.
     * The structure of the heap may be changed and will not be consolidated.
     * Running time: O(n)
     * @param new_primary_key new key value for node x
     * @param primary_key the value of the primary key to be found
     * @param secondary_key the value of the secondary key to be found. This param is optional. If missing, the search is based only on the primary key
     * @retun true if the key of the node was decreased, false otherwise
     * @exception invalid_argument Thrown if node x is an invalid pointer or if new_primary_key is equal to LOWEST_DOUBLE value or if new_primary_key is larger than x.primary_key value.
     */
    bool decreasePrimaryKey(const double new_primary_key, const double primary_key, const std::optional<double> secondary_key = std::optional<double>())
    {
        auto node = this->find(primary_key, secondary_key);
        if (node)
            this->decreasePrimaryKey(node, new_primary_key);
        return (node) ? true : false;
    }


    /**
     * Deletes a node from the Fibonacci heap given the reference to the node. 
     * The trees in the heap will be consolidated, if necessary. 
     * Running time: O(log n) amortized
     * WARNING: attention, if the node does not belong to the Fibonacci heap it is very probably that it will be corrupted after this operation
     * @param x node to remove from Fibonacci heap
     * @exception invalid_argument Thrown if node x is an invalid pointer
     */
    void deleteNode(const std::shared_ptr<FibonacciHeapNode<T>>& x)
    {
        // Check if Fibonacci heap node pointer is valid
        if (!x)
            throw std::invalid_argument("Empty Fibonacci heap node");

        try
        {
            // Make x as small as possible
            this->allow_minus_infinity = true;  // Important to do in order to avoid that decreasePrimaryKey throws an invalid_argument exception.
            this->decreasePrimaryKey(x, LOWEST_DOUBLE);
            this->allow_minus_infinity = false;

            // Remove the smallest node
            this->deleteMin();
        }
        catch (...)
        {
            this->allow_minus_infinity = false;
        }
    }


    /**
     * Deletes a node from the Fibonacci heap given the reference to the node.
     * It needs firstly to found the node.
     * The trees in the heap will be consolidated, if necessary.
     * Running time: O(n) amortized
     * WARNING: attention, if the node does not belong to the Fibonacci heap it is very probably that it will be corrupted after this operation
     * @param primary_key the value of the primary key to be found
     * @param secondary_key the value of the secondary key to be found. This param is optional. If missing, the search is based only on the primary key
     * @retun true if the key of the node was deleted, false otherwise
     * @exception invalid_argument Thrown if node x is an invalid pointer
     */
    bool deleteNode(const double primary_key, const std::optional<double> secondary_key = std::optional<double>())
    {
        auto node = this->find(primary_key, secondary_key);
        if (node)
            this->deleteNode(node);
        return (node) ? true : false;
    }
};
