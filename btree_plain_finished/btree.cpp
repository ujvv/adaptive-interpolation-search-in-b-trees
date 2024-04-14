#include "btree.hpp"
#include <cstring>
#include <map>
#include <vector>
#include <span>
#include <algorithm>
#include <iostream>

#define NODE_SIZE 4096 // 4KB BTree Node

struct PageSlot {
    uint16_t offset;
    uint16_t key_length;
    uint16_t value_length;
};

struct FenceKeySlot {
  uint16_t offset; // Offset of the key on the heap
  uint16_t len;    // Length of the key on the heap
};

struct BTreeNodeHeader {
    union {
        uint64_t right_most_child; // right most child of an inner node
        uint64_t next_leaf_node;   // right sibling of a leaf node
    };
    bool is_leaf;
    uint16_t count = 0;               // number of tuples
    uint16_t space_used = 0;          // total size of all keys & values
    uint16_t free_offset = NODE_SIZE; // prepend record from this offset
    uint16_t prefix_len = 0;
    uint16_t prefix_offset = 0;
    FenceKeySlot lower_fence = {0, 0};
    FenceKeySlot upper_fence = {0, 0};
};

class alignas(NODE_SIZE) BTreeNode : public BTreeNodeHeader
{
public:
    static constexpr uint32_t CONTENT_SIZE = NODE_SIZE - sizeof(BTreeNodeHeader);
    PageSlot slots[CONTENT_SIZE / sizeof(PageSlot)];

    BTreeNode() {right_most_child = 0; is_leaf = true;}

    BTreeNode(std::span<uint8_t> lower_fenceKey, std::span<uint8_t> upper_fenceKey) {
        uint16_t minLength = std::min(lower_fence.len, upper_fence.len);
        prefix_len = minLength;
        for (uint16_t i = 0; i < minLength; i++) {
            if (lower_fenceKey[i] != upper_fenceKey[i]) {
                prefix_len = i;
                break;
            }
        }

        if (prefix_len > 0) {
            prefix_offset = free_offset - prefix_len;
            std::memcpy(reinterpret_cast<uint8_t *>(this) + prefix_offset, lower_fenceKey.data(), prefix_len);
            free_offset = prefix_offset;
            space_used += prefix_len;
        }

        if (lower_fence.len - prefix_len > 0) {
            lower_fence.len = lower_fence.len - prefix_len;
            lower_fence.offset = free_offset - lower_fence.len;
            std::memcpy(reinterpret_cast<uint8_t *>(this) + lower_fence.offset, lower_fenceKey.data() + prefix_len, lower_fence.len);
            free_offset = lower_fence.offset;
            space_used += lower_fence.len;
        } else {
            lower_fence.len = 0;
            lower_fence.offset = 0;
        }

        if (upper_fence.len - prefix_len > 0) {
            upper_fence.len = upper_fence.len - prefix_len;
            upper_fence.offset = free_offset - upper_fence.len;
            std::memcpy(reinterpret_cast<uint8_t *>(this) + upper_fence.offset, upper_fenceKey.data() + prefix_len, upper_fence.len);
            free_offset = upper_fence.offset;
            space_used += upper_fence.len;
        } else {
            upper_fence.len = 0;
            upper_fence.offset = 0;
        }
    }

    // Sample: Read key & value at slot "pos"
    std::span<uint8_t> GetKey(uint16_t pos) {
        auto ptr = reinterpret_cast<uint8_t *>(this);
        return {ptr + slots[pos].offset, slots[pos].key_length};
    }

    std::vector<uint8_t> GetFullKey(uint16_t pos) {
    std::vector<uint8_t> key(slots[pos].key_length + prefix_len);

    if (prefix_len > 0) {
      std::memcpy(key.data(), this + lower_fence.offset, prefix_len);
    }
    if (slots[pos].key_length > 0) {
      std::memcpy(key.data() + prefix_len, reinterpret_cast<uint8_t *>(this) + slots[pos].offset, slots[pos].key_length);
    }
    return key;
  }

  std::vector<uint8_t> GetFenceKey(FenceKeySlot &fenceKey) {
    if (prefix_len == 0 || fenceKey.len == 0) {
      return {};
    }
    std::vector<uint8_t> key(prefix_len + fenceKey.len);
    if (prefix_len > 0) {
      std::memcpy(key.data(), reinterpret_cast<uint8_t *>(this) + prefix_offset, prefix_len);
    }
    if (fenceKey.len > 0) {
      std::memcpy(key.data() + prefix_len, reinterpret_cast<uint8_t *>(this) + fenceKey.offset, fenceKey.len);
    }
    return key;
  }

    std::span<uint8_t> GetValue(uint16_t pos) {
        auto ptr = reinterpret_cast<uint8_t *>(this);
        return {ptr + slots[pos].offset + slots[pos].key_length, slots[pos].value_length};
    }

};

inline std::vector<uint8_t> toByteVector(uint8_t *b, unsigned l) {
    return std::vector<uint8_t>(b, b + l);
}

struct BTree {
    BTreeNode *root;

    BTree() {
        root = new BTreeNode();
        root->is_leaf = true;
    }
};

namespace {
    BTreeNode *convertSpanToPointer(std::span<uint8_t> span) {
        BTreeNode *nodePtr;
        memcpy(&nodePtr, span.data(), span.size());
        return nodePtr;
    }
}

BTree *btree_create() { return new BTree(); };

namespace btree_destroy_helper {
    // Recursive helper function deleting each node
    void btree_delete(BTreeNode *node) {
        if (!node->is_leaf) {
            for (int i = 0; i > node->count - 1; i++) {
                BTreeNode *innerNode = convertSpanToPointer(node->GetValue(i));
                btree_delete(innerNode);
            }
            // At last delete right_most_child node
            BTreeNode *rightNode = reinterpret_cast<BTreeNode *>(node->right_most_child);
            btree_delete(rightNode);
        }
        delete node;
    }
}

// Destroy a tree created by btree_create
void btree_destroy(BTree *t) {
    if (!t->root->is_leaf) {
        btree_destroy_helper::btree_delete(t->root);
    }
    delete t;
}

namespace search {
    int binarySearch(BTreeNode *node, uint8_t *key, uint16_t keyLength) {
        std::span<uint8_t> keyWithoutPrefix(key + node->prefix_len, keyLength - node->prefix_len);
        int left = 0;
        int right;
        if (node->is_leaf) {
            right = node->count-1;
        } else {
            right = node->count-2;
        }
        int middle;

        while (left <= right) {
            middle = (left + right) / 2;
            if (std::lexicographical_compare(node->GetKey(middle).begin(), node->GetKey(middle).end(), keyWithoutPrefix.begin(), keyWithoutPrefix.end())) {
                left = middle + 1;
            } else {
                right = middle - 1;
            }
        }
        return left;
    }
}

bool btree_remove(BTree *tree, uint8_t *key, uint16_t keyLength) {
    BTreeNode *node = tree->root;
    while (!node->is_leaf) {
        // Find the child that may contain the key
        // Find right position which child Node to enter
        int i = search::binarySearch(node, key, keyLength);

        // Move to the next child
        if (i == node->count - 1) {
            // right_most_child:
            node = reinterpret_cast<BTreeNode *>(node->right_most_child);
        } else {
            // child at slot position i
            node = convertSpanToPointer(node->GetValue(i));
        }
    } // Now node is a leaf

    // Find the key in this leaf
    int i = search::binarySearch(node, key, keyLength);
    if (i < node->count && std::equal(node->GetKey(i).begin(), node->GetKey(i).end(), key, key + keyLength)) {
        // Shift the remaining slots down (deletes slot of key in the process)
        for (int j = i; j < node->count - 1; ++j) {
            node->slots[j] = node->slots[j + 1];
        }
        // Decrement the count
        node->count--;
        // Free the space of the deleted slot
        node->space_used -= sizeof(PageSlot);

        return true;
    }
    return false; // Key not found
}

// Helper functions for insert
namespace {

    void updateChildPointer(BTreeNode *parent, uint8_t *oldKey, uint16_t oldKeyLength, BTreeNode *rightChild) {
        // Find the insertion position
        int pos = search::binarySearch(parent, oldKey, oldKeyLength);
        if (pos >= parent->count - 1) {
            parent->right_most_child = reinterpret_cast<uint64_t>(rightChild);
            return;
            }
        
        // Update the value (ChildPointer)
        std::memcpy(reinterpret_cast<uint8_t *>(parent) + parent->slots[pos].offset + parent->slots[pos].key_length, reinterpret_cast<uint8_t *>(&rightChild), parent->slots[pos].value_length);
    }

    bool insertInInnerNode(BTreeNode *node, uint8_t *key, uint16_t keyLength, BTreeNode *leftChild) {
        uint16_t shortenedkeyLength = keyLength - node->prefix_len;
        
        // Check if there is enough space in the inner node
        if (node->space_used + shortenedkeyLength + sizeof(leftChild) + sizeof(PageSlot) > BTreeNode::CONTENT_SIZE) {
            return false; // not enough space
        }

        // Find the insertion position
        // (right_most_child does not need to be checked because function is always called with updateChildPointer() which node always lies to the right of current leftChild)
        int pos = search::binarySearch(node, key, keyLength);

        // Shift slots to make room for new slot
        for (int i = node->count - 2; i >= pos; --i) {
            node->slots[i + 1] = node->slots[i];
        }

        // Set up the new slot for the key
        PageSlot newSlot;
        newSlot.offset = node->free_offset - shortenedkeyLength - sizeof(leftChild);
        newSlot.key_length = shortenedkeyLength;
        newSlot.value_length = sizeof(leftChild);
        node->slots[pos] = newSlot;

        // Copy the key and child pointer into the node
        std::memcpy(reinterpret_cast<uint8_t *>(node) + newSlot.offset, key + node->prefix_len, shortenedkeyLength);
        std::memcpy(reinterpret_cast<uint8_t *>(node) + newSlot.offset + shortenedkeyLength, reinterpret_cast<uint8_t *>(&leftChild), sizeof(leftChild));

        // Update the node header
        node->count++;
        node->space_used += shortenedkeyLength + sizeof(leftChild) + sizeof(PageSlot);
        node->free_offset = newSlot.offset;

        return true;
    }

    bool insertInLeaf(BTreeNode *leaf, uint8_t *key, uint16_t keyLength, uint8_t *value, uint16_t valueLength) {
        uint16_t shortenedkeyLength = keyLength - leaf->prefix_len;
        // Check if there is enough space in the leaf node
        if (leaf->space_used + shortenedkeyLength + valueLength + sizeof(PageSlot) > BTreeNode::CONTENT_SIZE) {
            // Edge Cases Empty Leaf and count==1
            if(leaf->count == 0) {
                leaf->count = 0;
                leaf->space_used = 0;
                leaf->free_offset = NODE_SIZE;
            } else if(leaf->count == 1) {
                leaf->count = 0;
                leaf->space_used = 0;
                leaf->free_offset = NODE_SIZE;
                auto fullKey = leaf->GetFullKey(0);
                insertInLeaf(leaf, fullKey.data(), fullKey.size(), leaf->GetValue(0).data(), leaf->GetValue(0).size());
            } else {
                return false; // not enough space
            }
        }

        // Prepare the new slot
        PageSlot newSlot;
        newSlot.offset = leaf->free_offset - shortenedkeyLength - valueLength;
        newSlot.key_length = shortenedkeyLength;
        newSlot.value_length = valueLength;

        // Move the key and value into position
        std::memcpy(reinterpret_cast<uint8_t *>(leaf) + newSlot.offset, key + leaf->prefix_len, shortenedkeyLength);
        std::memcpy(reinterpret_cast<uint8_t *>(leaf) + newSlot.offset + shortenedkeyLength, value, valueLength);

        // Find insertion position for sorted order
        int pos;
        // Catch edge case
        if (leaf->count == 0) {
            pos = 0;
        } else {
            pos = search::binarySearch(leaf, key, keyLength);
        }
        // Check if key already exists
        if (pos < leaf->count && leaf->count > 0 && std::equal(key + leaf->prefix_len, key + shortenedkeyLength, leaf->GetKey(pos).begin(), leaf->GetKey(pos).end())) {
            // Decrement count because it will be updated later
            leaf->count--;
            // Update already existing slot
            leaf->slots[pos].offset = leaf->free_offset - shortenedkeyLength - valueLength;
            leaf->slots[pos].key_length = shortenedkeyLength;
            leaf->slots[pos].value_length = valueLength;
        } else {
            // Move slots if necessary
            for (int i = leaf->count - 1; i >= pos; --i) {
                leaf->slots[i + 1] = leaf->slots[i];
            }
            // Insert the new slot
            leaf->slots[pos] = newSlot;
        }
        // Update leaf node header
        leaf->count++;
        leaf->space_used += shortenedkeyLength + valueLength + sizeof(PageSlot);
        leaf->free_offset = newSlot.offset;

        return true;
    }

    std::pair<BTreeNode*, std::vector<uint8_t>> splitLeaf(BTreeNode *oldLeaf) {
        // Determine the split point
        int splitIndex = (oldLeaf->count + 1) / 2;  // This gives the index at which pos to split. Entry at splitIndex goes into newLeaf
        int newCount = oldLeaf->count - splitIndex; // Number of slots for the new leaf

        std::vector<uint8_t> lower_fence = oldLeaf->GetFullKey(splitIndex);
        std::vector<uint8_t> upper_fence = oldLeaf->GetFenceKey(oldLeaf->upper_fence);
        BTreeNode *newLeaf = new BTreeNode(lower_fence, upper_fence);
        newLeaf->is_leaf = true;

        // Transfers entries from the split point to the end into the new leaf
        for (int i = 0; i < newCount; ++i) {
            auto key = oldLeaf->GetFullKey(i + splitIndex);
            auto value = oldLeaf->GetValue(i + splitIndex);
            insertInLeaf(newLeaf, key.data(), key.size(), value.data(), value.size());
        }

        lower_fence = oldLeaf->GetFenceKey(oldLeaf->lower_fence);
        upper_fence = oldLeaf->GetFullKey(splitIndex);
        BTreeNode *updatedoldLeaf = new BTreeNode(lower_fence, upper_fence);
        updatedoldLeaf->is_leaf = true;

        for (int i = 0; i < splitIndex; ++i) {
            auto keytoinsert = oldLeaf->GetFullKey(i);
            auto valuetoinsert = oldLeaf->GetValue(i);
            insertInLeaf(updatedoldLeaf, keytoinsert.data(), keytoinsert.size(), valuetoinsert.data(), valuetoinsert.size());
        }
        updatedoldLeaf->next_leaf_node = oldLeaf->next_leaf_node;
        
        memcpy(reinterpret_cast<uint8_t *>(oldLeaf), reinterpret_cast<uint8_t *>(updatedoldLeaf), NODE_SIZE);
        delete updatedoldLeaf;

        newLeaf->next_leaf_node = oldLeaf->next_leaf_node;             // New leaf takes the next pointer of the old leaf
        oldLeaf->next_leaf_node = reinterpret_cast<uint64_t>(newLeaf); // Old leaf now points to the new leaf

        // Save middle key so it can be pushed to the parent node
        auto middleKeyIndex = oldLeaf->count - 1;
        auto fullMiddleKey = oldLeaf->GetFullKey(middleKeyIndex);

        return std::make_pair(newLeaf, fullMiddleKey);
    }

    std::pair<BTreeNode*, std::vector<uint8_t>> splitInnerNode(BTreeNode *oldNode) {
        int splitIndex = (oldNode->count + 1) / 2;  // This gives the index at which pos to split. Entry at splitIndex goes into newLeaf
        int newCount = oldNode->count - splitIndex; // Number of slots for the new node

        std::vector<uint8_t> lower_fence = oldNode->GetFenceKey(oldNode->lower_fence);
        std::vector<uint8_t> upper_fence = oldNode->GetFullKey(splitIndex);
        BTreeNode *newNode = new BTreeNode(lower_fence, upper_fence);
        newNode->is_leaf = false;

        // Initialize headers for the new node
        newNode->count = newCount;

        // Iterate over the slots and copy them individually to the new node
        for (int i = 0; i < newCount; ++i) {
            if (i == newCount - 1 && newCount > 1) {
                // Set right_most_child for new node at last iteration
                newNode->right_most_child = oldNode->right_most_child;
                break;
            }
            auto key = oldNode->GetFullKey(splitIndex + i);
            std::span<uint8_t> keyWithoutPrefix(key.data() + newNode->prefix_len, key.size() - newNode->prefix_len);
            //memcpy(keyWithoutPrefix.data(), key.data() + newNode->prefix_len, key.size() - newNode->prefix_len);
            auto value = oldNode->GetValue(splitIndex + i);

            PageSlot slotToCopy = oldNode->slots[i + splitIndex];
            slotToCopy.key_length = keyWithoutPrefix.size();
            slotToCopy.value_length = value.size();
            slotToCopy.offset = newNode->free_offset - slotToCopy.key_length - slotToCopy.value_length;
            newNode->slots[i] = slotToCopy;

            std::memcpy(reinterpret_cast<uint8_t *>(newNode) + slotToCopy.offset, keyWithoutPrefix.data(), slotToCopy.key_length);
            std::memcpy(reinterpret_cast<uint8_t *>(newNode) + slotToCopy.offset + slotToCopy.key_length, value.data(), slotToCopy.value_length);

            newNode->space_used += slotToCopy.key_length + slotToCopy.value_length + sizeof(PageSlot);
            newNode->free_offset = slotToCopy.offset;
        }

        lower_fence = oldNode->GetFenceKey(oldNode->lower_fence);
        upper_fence = oldNode->GetFullKey(splitIndex);
        BTreeNode *updatedoldNode = new BTreeNode(lower_fence, upper_fence);
        updatedoldNode->is_leaf = false;
        updatedoldNode->count = splitIndex;
        
        std::vector<uint8_t> fullmiddleKey;
        for (int i = 0; i < splitIndex; ++i) {
            if (i == splitIndex - 1 && splitIndex > 1) {
                fullmiddleKey = oldNode->GetFullKey(i);
                
                // Update right_most_child for old Node
                updatedoldNode->right_most_child = reinterpret_cast<uint64_t>(convertSpanToPointer(oldNode->GetValue(i)));
                break;
            }
            auto keyToCopy = oldNode->GetFullKey(i);
            auto valueToCopy = oldNode->GetValue(i);
            std::span<uint8_t> keyWithoutPrefix(keyToCopy.data() + updatedoldNode->prefix_len, keyToCopy.size() - updatedoldNode->prefix_len);
            
            PageSlot slotToCopy;
            slotToCopy.key_length = keyWithoutPrefix.size();
            slotToCopy.value_length = valueToCopy.size();
            updatedoldNode->slots[i] = slotToCopy;

            std::memcpy(reinterpret_cast<uint8_t *>(updatedoldNode) + updatedoldNode->free_offset - updatedoldNode->slots[i].key_length - updatedoldNode->slots[i].value_length, keyWithoutPrefix.data(), keyWithoutPrefix.size());
            std::memcpy(reinterpret_cast<uint8_t *>(updatedoldNode) + updatedoldNode->free_offset - updatedoldNode->slots[i].value_length, valueToCopy.data(), valueToCopy.size());

            updatedoldNode->slots[i].offset = updatedoldNode->free_offset - slotToCopy.key_length - slotToCopy.value_length;
            updatedoldNode->space_used += oldNode->slots[i].key_length + oldNode->slots[i].value_length + sizeof(PageSlot);
            updatedoldNode->free_offset = updatedoldNode->slots[i].offset;
        }
        memcpy(reinterpret_cast<uint8_t *>(oldNode), reinterpret_cast<uint8_t *>(updatedoldNode), NODE_SIZE);
        delete updatedoldNode;
        return std::make_pair(newNode, fullmiddleKey);
    }

} // End of namespace (Helper functions for insert)


void btree_insert(BTree *tree, uint8_t *key, uint16_t keyLength, uint8_t *value,
                  uint16_t valueLength) {

    BTreeNode *node = tree->root;
    BTreeNode *parent = nullptr;

    // Stack to keep track of the path taken to reach the leaf (for backtracking in case of splits)
    std::vector<BTreeNode *> pathStack;

    // Will save the rightNode (newNode) after a node split and the separator (middle) key for the parent node
    std::pair<BTreeNode *, std::vector<uint8_t>> pairNodeKey;
    while (!node->is_leaf) {
        pathStack.push_back(node); // Push current node to stack

        // Find the child that may contain the key
        int i = search::binarySearch(node, key, keyLength);
        // Move to the next child
        parent = node;
        if (i >= node->count - 1) {
            node = reinterpret_cast<BTreeNode *>(node->right_most_child);
        } else {
            node = convertSpanToPointer(node->GetValue(i));
        }
    } // Node is a leaf now

    // Try to insert value into leaf if not full
    if (!insertInLeaf(node, key, keyLength, value, valueLength)) {
        // Leaf was full, need to split
        std::vector<uint8_t> fullMiddleKey;
        
        pairNodeKey = splitLeaf(node);
        BTreeNode *newLeaf = pairNodeKey.first;
        fullMiddleKey = pairNodeKey.second;
        newLeaf->is_leaf = true;

        // Get right most key from right child node because that used to be old key in parent node (needed for updateChildPointer)
        auto oldFullKey = newLeaf->GetFullKey(newLeaf->count-1);
        uint8_t *oldKey = oldFullKey.data();
        uint16_t oldKeyLength = oldFullKey.size();
        
        // Insert key in correct leaf after split
        if (!std::lexicographical_compare(fullMiddleKey.data(), fullMiddleKey.data() + fullMiddleKey.size(), key, key + keyLength)) {
            insertInLeaf(node, key, keyLength, value, valueLength);
        } else {
            insertInLeaf(newLeaf, key, keyLength, value, valueLength);
        }

        if (parent == nullptr) {
            // Need to create new root
            BTreeNode *newRoot = new BTreeNode();
            newRoot->is_leaf = false;
            tree->root = newRoot;
            insertInInnerNode(newRoot, fullMiddleKey.data(), fullMiddleKey.size(), node);
            // New roots right most child is now newLeaf
            newRoot->right_most_child = reinterpret_cast<uint64_t>(newLeaf);
            newRoot->count++;

        } else {
            // Set right child node
            BTreeNode *rightNode = newLeaf;
            // Insert middleKey and nodePointer to the parent
            // Till parent node does not need to be split anymore or root is reached
            while (true) {
                updateChildPointer(parent, oldKey, oldKeyLength, rightNode);
                bool insertSuccesful;
                insertSuccesful = insertInInnerNode(parent, fullMiddleKey.data(), fullMiddleKey.size(), node);
                if (insertSuccesful) {                 
                    break;
                } else { // Parent is full, split the parent
                    // Store current middleyKey that still needs to be inserted because it will be updated with a new middleKey in split
                    uint8_t *oldMiddleKey = new uint8_t[fullMiddleKey.size()];
                    std::memcpy(oldMiddleKey, fullMiddleKey.data(), fullMiddleKey.size());
                    auto oldmiddleKeyLength = fullMiddleKey.size();

                    pairNodeKey = splitInnerNode(parent);
                    BTreeNode *newInnerNode = pairNodeKey.first;
                    fullMiddleKey = pairNodeKey.second;
                    newInnerNode->is_leaf = false;

                    // Insert key stored in oldmiddleKey in correct node after parent split
                    if (!std::lexicographical_compare(fullMiddleKey.data(), fullMiddleKey.data() + fullMiddleKey.size(), oldMiddleKey, oldMiddleKey + oldmiddleKeyLength)) {
                        insertInInnerNode(parent, oldMiddleKey, oldmiddleKeyLength, node);
                    } else {
                        insertInInnerNode(newInnerNode, oldMiddleKey, oldmiddleKeyLength, node);
                    }
                    
                    delete[] oldMiddleKey;

                    if (pathStack.size() == 1) {
                        // New root necessary
                        BTreeNode *newRoot = new BTreeNode();
                        newRoot->is_leaf = false;
                        tree->root = newRoot;
                        insertInInnerNode(newRoot, fullMiddleKey.data(), fullMiddleKey.size(), parent);
                        // New roots right most child is now newInnerNode (updateChildPointer not necessary now)
                        newRoot->right_most_child = reinterpret_cast<uint64_t>(newInnerNode);
                        newRoot->count++;
                        
                        break;
                    } else {
                        // Move up one level in the tree
                        pathStack.pop_back();

                        // Set oldKey that ChildPointer can be updated in new parent node at the right place
                        oldFullKey = newInnerNode->GetFullKey(newInnerNode->count-2);
                        oldKey = oldFullKey.data();
                        oldKeyLength = oldFullKey.size();

                        // Update node, rightNode and parent
                        node = parent;
                        rightNode = newInnerNode;
                        parent = pathStack.back();
                    }
                }
            }
        }
    }
}

uint8_t *btree_lookup(BTree *tree, uint8_t *key, uint16_t keyLength,
                      uint16_t &payloadLengthOut) {
    BTreeNode *current = tree->root;

    // Traverse the tree with current starting from the root node
    while (current != nullptr) {
        if (current->is_leaf) {
            // Current is a leaf
            // Iterate over keys to find the match
            int i = search::binarySearch(current, key, keyLength);
            auto k = current->GetFullKey(i);
            if (std::equal(k.begin(), k.end(), key, key + keyLength)) {
                // Key match found, set the payload length and return the value pointer
                payloadLengthOut = current->slots[i].value_length;
                return current->GetValue(i).data();
            }
            // Key is not present in this leaf node
            return nullptr;
        } else {
            // Current is an internal node
            // Find the child pointer to follow
            int i = search::binarySearch(current, key, keyLength);
            if (i >= current->count - 1) {
                // If it comes to last iteration, descend to right most child
                current = reinterpret_cast<BTreeNode *>(current->right_most_child);
            } else {
                current = convertSpanToPointer(current->GetValue(i));
            }
        }
    }
    return nullptr; // Key is not present in the tree
}

void btree_scan(BTree *tree, uint8_t *key, unsigned keyLength, uint8_t *keyOut,
                const std::function<bool(unsigned int, uint8_t *, unsigned int)>
                    &found_callback) {
    BTreeNode *current = tree->root;

    // Traverse the tree starting from the root to find the leaf node where scanning will start
    while (current && !current->is_leaf) {
        // At each internal node, find the child node that may contain the key or the next larger key
        int i = search::binarySearch(current, key, keyLength);
        // If i is at the last child, needs to check right_most_child
        if (i >= current->count - 1 && current->count > 1) {
            current = reinterpret_cast<BTreeNode *>(current->right_most_child);
        } else {
            current = convertSpanToPointer(current->GetValue(i));
        }
    }

    // Now `current` should be the leaf node where scanning will start
    while (current) {
        for (int i = 0; i < current->count; ++i) {
            auto k = current->GetFullKey(i);
            if (!std::lexicographical_compare(k.begin(), k.end(), key, key + keyLength)) {
                // Found a key greater than or equal to the input key, invoke the callback
                std::memcpy(keyOut, k.data(), k.size());
                auto value = current->GetValue(i);
                if (!found_callback(k.size(), value.data(), value.size())) {
                    // If the callback returns false, stop scanning
                    return;
                }
            }
        }
        // Move to the next leaf node
        current = reinterpret_cast<BTreeNode *>(current->next_leaf_node);
    }
}
