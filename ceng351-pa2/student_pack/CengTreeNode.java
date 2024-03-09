import java.awt.Color;

public abstract class CengTreeNode {
    static protected Integer order;
    private CengTreeNode parent;
    protected CengNodeType type; // Type needs to be set for proper GUI. Check CengNodeType.java.

    // GUI Accessors - do not modify
    public Integer level;
    public Color color;

    // Any extra attributes, if necessary

    public CengTreeNode(CengTreeNode parent) {
        this.parent = parent;
        this.color = CengGUI.getRandomBorderColor();

        // TODO: Extra initializations, if necessary.
    }

    // Getters-Setters - Do not modify
    public CengTreeNode getParent() {
        return parent;
    }

    public void setParent(CengTreeNode parent) {
        this.parent = parent;
    }

    public CengNodeType getType() {
        return type;
    }

    // GUI Methods - Do not modify
    public Color getColor() {
        return this.color;
    }

    // Extra Functions
    public abstract int getNodeKey();

    public abstract void printNode(int tabCount);

}
