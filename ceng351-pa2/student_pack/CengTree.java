import java.util.ArrayList;

public class CengTree {
    public CengTreeNode root;
    // Any extra attributes...

    public CengTree(Integer order) {
        CengTreeNode.order = order;
        // TODO: Initialize the class
        root = new CengTreeNodeLeaf(null);
        root.type = CengNodeType.Leaf;
    }

    public void addBook(CengBook book) {
        // TODO: Insert Book to Tree
        Integer searchKey = book.getBookID();

        CengTreeNode currentNode = root;
        while (currentNode.getType() != CengNodeType.Leaf) {
            currentNode = ((CengTreeNodeInternal) currentNode).getChildNode(searchKey);
        }

        CengTreeNodeInternal newRoot = ((CengTreeNodeLeaf) currentNode).addBookToLeaf(book);
        root = newRoot == null ? root : newRoot;
    }

    public ArrayList<CengTreeNode> searchBook(Integer bookID) {
        // TODO: Search within whole Tree, return visited nodes.
        // Return null if not found.
        ArrayList<CengTreeNode> visitedNodes = new ArrayList<>();

        CengTreeNode currentNode = root;
        int tabCount = 0;
        while (currentNode.getType() != CengNodeType.Leaf) {
            visitedNodes.add(currentNode);
            currentNode = ((CengTreeNodeInternal) currentNode).getChildNode(bookID);
        }
        visitedNodes.add(currentNode); // Add the leaf node

        if (!((CengTreeNodeLeaf) currentNode).containsBook(bookID)) {
            System.out.println("Could not find " + bookID + ".");
            return null;
        } else {
            for (int i = 0; i < visitedNodes.size() - 1; i++) {
                CengTreeNodeInternal node = (CengTreeNodeInternal) visitedNodes.get(i);
                node.searchPrintNode(tabCount++);
            }
            ((CengTreeNodeLeaf) currentNode).searchPrintNode(tabCount++, bookID);
            return visitedNodes;
        }
    }

    public void printTree() {
        // TODO: Print the whole tree to console
        root.printNode(0);
    }

    // Any extra functions...
}
