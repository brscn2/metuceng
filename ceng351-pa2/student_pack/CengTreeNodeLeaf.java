import java.util.ArrayList;
import java.util.Comparator;

public class CengTreeNodeLeaf extends CengTreeNode {
    private ArrayList<CengBook> books;
    // TODO: Any extra attributes

    public CengTreeNodeLeaf(CengTreeNode parent) {
        super(parent);
        this.type = CengNodeType.Leaf;
        this.books = new ArrayList<>();
        // TODO: Extra initializations
    }

    public CengTreeNodeLeaf(CengTreeNode parent, ArrayList<CengBook> bookList) {
        super(parent);
        this.type = CengNodeType.Leaf;
        this.books = bookList;
    }

    // GUI Methods - Do not modify
    public int bookCount() {
        return books.size();
    }

    public Integer bookKeyAtIndex(Integer index) {
        if (index >= this.bookCount()) {
            return -1;
        } else {
            CengBook book = this.books.get(index);

            return book.getBookID();
        }
    }

    // Extra Functions
    public boolean isFull() {
        return books.size() > (order * 2);
    }

    public boolean containsBook(Integer key) {
        for (CengBook book : books) {
            if (book.getBookID().equals(key))
                return true;
        }

        return false;
    }

    // public boolean isMinimum() {
    // return books.size() <= order;
    // }

    public CengTreeNodeInternal addBookToLeaf(CengBook book) {
        this.books.add(book);
        books.sort(Comparator.comparingInt(CengBook::getBookID));

        CengTreeNodeInternal newRoot = null;

        if (isFull()) {
            CengTreeNodeLeaf newLeaf = new CengTreeNodeLeaf(this.getParent(),
                    this.getSplitBooks());

            if (getParent() != null)
                newRoot = ((CengTreeNodeInternal) getParent()).copyLeaf(newLeaf);
            else {
                newRoot = new CengTreeNodeInternal(null);
                newRoot.addChild(this);
                this.setParent(newRoot);
                newRoot.copyLeaf(newLeaf);
            }
        }

        return newRoot;
    }

    public ArrayList<CengBook> getSplitBooks() {
        ArrayList<CengBook> splitBooks = new ArrayList<>();

        // MOVE THE LAST d + 1 books to a new arraylist that will be used for the split
        // node.
        for (int i = CengTreeNode.order; i < books.size(); i++) {
            splitBooks.add(books.get(i));
        }

        // Remove moved books from the list
        for (int i = CengTreeNode.order + 1; i > 0; i--) {
            books.remove(books.size() - 1);
        }

        return splitBooks;
    }

    public int getNodeKey() {
        return books.get(0).getBookID();
    }

    public CengBook getBookById(Integer bookId) {
        for (CengBook book : books) {
            if (book.getBookID().equals(bookId))
                return book;
        }
        return null;
    }

    public void searchPrintNode(int tabCount, Integer bookId) {
        String tabString = "\t".repeat(Math.max(0, tabCount));

        CengBook book = getBookById(bookId);
        System.out.println(tabString + "<record>" + book.getBookID() + "|" + book.getBookTitle() + "|"
                + book.getAuthor() + "|" + book.getGenre() + "</record>");
    }

    public void printNode(int tabCount) {
        String tabString = "\t".repeat(Math.max(0, tabCount));

        System.out.println(tabString + "<data>");
        for (CengBook book : books) {
            System.out.println(tabString + "<record>" + book.getBookID() + "|" + book.getBookTitle() + "|"
                    + book.getAuthor() + "|" + book.getGenre() + "</record>");
        }
        System.out.println(tabString + "</data>");
    }
    // public void setBooks(ArrayList<CengBook> bl) {
    // this.books = bl;
    // }
}
