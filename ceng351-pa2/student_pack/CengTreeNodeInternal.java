import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class CengTreeNodeInternal extends CengTreeNode {
    private ArrayList<Integer> keys;
    private ArrayList<CengTreeNode> children;

    public CengTreeNodeInternal(CengTreeNode parent) {
        super(parent);
        this.type = CengNodeType.Internal;
        this.keys = new ArrayList<>();
        this.children = new ArrayList<>();
        // TODO: Extra initializations, if necessary.
    }

    // GUI Methods - Do not modify
    public ArrayList<CengTreeNode> getAllChildren() {
        return this.children;
    }

    public Integer keyCount() {
        return this.keys.size();
    }

    public Integer keyAtIndex(Integer index) {
        if (index >= this.keyCount() || index < 0) {
            return -1;
        } else {
            return this.keys.get(index);
        }
    }

    // Extra Functions
    public CengTreeNode getChildNode(Integer searchKey) {
        int keyIndex = 0, size = keys.size();
        while (keyIndex < size && searchKey >= keys.get(keyIndex)) {
            keyIndex++;
        }
        return children.get(keyIndex);
    }

    public CengTreeNodeInternal copyLeaf(CengTreeNodeLeaf childNode) {
        addKey(childNode.getNodeKey());
        addChild(childNode);
        childNode.setParent(this);

        if (isFull())
            return splitNode();

        return null;
    }

    public CengTreeNodeInternal pushInternal(int key, CengTreeNodeInternal newInternalNode) {
        addKey(key);
        addChild(newInternalNode);
        newInternalNode.setParent(this);

        if (isFull())
            return splitNode();

        return null;
    }

    public boolean isFull() {
        return keys.size() > (2 * order);
    }

    // public boolean isMinimum() {
    // return keys.size() <= order;
    // }

    public int getNodeKey() {
        return keys.get(0);
    }

    public void addChild(CengTreeNode child) {
        children.add(child);
        children.sort(Comparator.comparingInt(CengTreeNode::getNodeKey));
    }

    public void addKey(Integer key) {
        keys.add(key);
        Collections.sort(keys);
    }

    private CengTreeNodeInternal splitNode() {
        CengTreeNodeInternal newInternalNode = new CengTreeNodeInternal(null);
        int middleKey = keys.get(CengTreeNode.order);

        CengTreeNode child = children.get(CengTreeNode.order + 1);
        child.setParent(newInternalNode);
        newInternalNode.children.add(child);

        for (int i = CengTreeNode.order + 1; i < keys.size(); i++) {
            newInternalNode.keys.add(keys.get(i));
            child = children.get(i + 1);
            child.setParent(newInternalNode);
            newInternalNode.children.add(child);
        }
        // System.out.println("--------------------------");

        for (int i = CengTreeNode.order + 1; i > 0; i--) {
            keys.remove(keys.size() - 1);
            children.remove(children.size() - 1);
        }

        if (this.getParent() == null) {
            CengTreeNodeInternal newNodeForParent = new CengTreeNodeInternal(null);
            newNodeForParent.keys.add(middleKey);
            newNodeForParent.children.add(this);
            newNodeForParent.children.add(newInternalNode);
            this.setParent(newNodeForParent);
            newInternalNode.setParent(newNodeForParent);
            return newNodeForParent;
        } else {
            return ((CengTreeNodeInternal) this.getParent()).pushInternal(middleKey, newInternalNode);
        }
    }

    public void searchPrintNode(int tabCount) {
        String tabString = "\t".repeat(Math.max(0, tabCount));

        System.out.println(tabString + "<index>");
        for (Integer key : keys) {
            System.out.println(tabString + key);
        }
        System.out.println(tabString + "</index>");
    }
    public void printNode(int tabCount) {
        String tabString = "\t".repeat(Math.max(0, tabCount));

        System.out.println(tabString + "<index>");
        for (Integer key : keys) {
            System.out.println(tabString + key);
        }
        System.out.println(tabString + "</index>");

        for (CengTreeNode node : children) {
            node.printNode(tabCount + 1);
        }
    }
}
