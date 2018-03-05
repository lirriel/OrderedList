#include <cstdlib>
#include <vector>

//=============================================================================
//== NodeSkipList =============================================================
//=============================================================================

template <class Value, class Key, int numLevels>
void NodeSkipList<Value,Key,numLevels>::clear(void)
{
	for (int i = 0; i < numLevels; ++i)
	{
        NodeSkipListAbstract<Value, Key, numLevels, NodeSkipList<Value, Key, numLevels> >::m_nextjump[i] = 0;
	}
    NodeSkipListAbstract<Value, Key, numLevels, NodeSkipList<Value, Key, numLevels> >::m_levelHighest = -1;
}

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(void)
{
	clear();
}

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(Key key)
{
	clear();

    NodeSkipListAbstract<Value, Key, numLevels, NodeSkipList<Value, Key, numLevels> >::m_key = key;
}

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(Key key, Value value)
{
	clear();

    NodeSkipListAbstract<Value, Key, numLevels, NodeSkipList<Value, Key, numLevels> >::m_key = key;
    NodeSkipListAbstract<Value, Key, numLevels, NodeSkipList<Value, Key, numLevels> >::m_value = value;
}
//=============================================================================
//== End of: NodeSkipList =====================================================
//=============================================================================

//=============================================================================
//== SkipList =================================================================
//=============================================================================
template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::SkipList(double probability)
{
	m_probability = probability;
	for (int i = 0; i < numLevels; ++i)
	{
		// Lets use m_pPreHead as a final sentinel element
        OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead->m_nextjump[i] =
                OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead;
	}
    OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead->m_levelHighest = numLevels-1;
}

// Put your code here

/// Get previous elements for each level including the dense one by \c key
template <class Value, class Key, int numLevels>
std::vector<typename SkipList<Value, Key, numLevels>::TypeNode*> getPredecessors(Key key,
																				 SkipList<Value, Key, numLevels> *list)
{
	// create vector for elements and reserve memory
	std::vector<typename SkipList<Value, Key, numLevels>::TypeNode*> predecessors;
	predecessors.reserve(numLevels + 2);

	typename SkipList<Value, Key, numLevels>::TypeNode* current = list->getPreHead();

	// look for a suitable element on each additional level
	for (int i = numLevels; i-- > 0;)
	{
		while (current->m_nextjump[i] != list->getPreHead() && current->m_nextjump[i] != 0
               && current->m_nextjump[i]->m_key < key)
			current = current->m_nextjump[i];

		predecessors[i + 1] = current;
	}

	// look for a suitable element on the lowest level
	while (current->m_next != list->getPreHead() && current->m_next->m_key < key)
		current = current->m_next;
	predecessors[0] = current;

	return predecessors;
}


template <class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::insert(Value value, Key key)
{
	// get vector of previous nodes for each level
	std::vector<typename SkipList<Value, Key, numLevels>::TypeNode*> predecessors = getPredecessors(key, this);

	// previous for the dense level
	typename SkipList<Value, Key, numLevels>::TypeNode* findNode = predecessors[0];

	// create new node
	typename SkipList<Value, Key, numLevels>::TypeNode* newNode =
			new typename SkipList<Value, Key, numLevels>::TypeNode(key, value);

	// insert in the lowest level
	newNode->m_next = findNode->m_next;
	findNode->m_next = newNode;

	// insert into additional levels by chance
	srand(time(NULL));
	int i = 0;
	while (i < numLevels && (rand()%100)/(100 * 1.0) <= m_probability)
	{
		// get possible previous element for <i+1> level
		findNode = predecessors[i + 1];

		// insert new node
		newNode->m_nextjump[i] = findNode->m_nextjump[i];
		findNode->m_nextjump[i] = newNode;

		i++;
	}
}


template <class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::remove(TypeNode *node)
{
    // if node doesn't exist or doesn't have next node
    if (!node || node == OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead)
        throw std::invalid_argument("Nothing to remove!");

    // find predecessors on each level to remove \c toRemove from those levels
    std::vector<typename SkipList<Value, Key, numLevels>::TypeNode*> predecessors =
            getPredecessors(node->m_key, this);

    typename SkipList<Value, Key, numLevels>::TypeNode* currentNode;

    currentNode = predecessors[0];

    // if \c node doesn't belong to this list
    if (currentNode->m_next->m_key != node->m_key && currentNode->m_next->m_value != node->m_value)
        throw std::invalid_argument("This node doesn't belong here!");

    currentNode->m_next = node->m_next;

    // go through additional levels
    for (int i = 0; i < numLevels; ++i)
    {
        currentNode = predecessors[i + 1];
        currentNode->m_nextjump[i] = node->m_nextjump[i];
    }

    delete  node;
}


template <class Value, class Key, int numLevels>
typename SkipList<Value, Key, numLevels>::TypeNode* SkipList<Value, Key, numLevels>::findLastLessThan(Key key) const
{
	typename SkipList<Value, Key, numLevels>::TypeNode* current =
            OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead;

	// find last element lower with m_key lower than key on additional levels
	for (int i = numLevels; i-- > 0;)
		while (current->m_nextjump[i] != OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead
			   && current->m_nextjump[i] != 0
               && current->m_nextjump[i]->m_key < key)
			current = current->m_nextjump[i];

	// and consider the dense level
	while (current->m_next != OrderedList < Value, Key, NodeSkipList<Value, Key, numLevels> >::m_pPreHead
		   && current->m_next->m_key < key)
		current = current->m_next;

	return current;
}


template <class Value, class Key, int numLevels>
typename SkipList<Value, Key, numLevels>::TypeNode * SkipList<Value, Key, numLevels>::findFirst(Key key) const
{
	// find last element with key lower than needed
	typename SkipList<Value, Key, numLevels>::TypeNode* lastLessThan = findLastLessThan(key);

	if (lastLessThan->m_next->m_key != key)
		return nullptr;
	else
		return lastLessThan->m_next;
}

//=============================================================================
//== End of: SkipList =========================================================
//=============================================================================
