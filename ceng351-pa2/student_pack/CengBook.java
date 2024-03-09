public class CengBook
{
    private Integer bookID;
    private String bookTitle;
    private String author;
    private String genre;

    public CengBook(Integer bookID, String bookTitle, String author, String genre)
    {
        this.bookID = bookID;
        this.bookTitle = bookTitle;
        this.author = author;
        this.genre = genre;
    }

    // Getters

    public Integer getBookID()
    {
        return bookID;
    }
    public String getBookTitle()
    {
        return bookTitle;
    }
    public String getAuthor()
    {
        return author;
    }
    public String getGenre()
    {
        return genre;
    }

    // GUI method - do not modify
    public String fullName()
    {
        return "" + getBookID() + "|" + getBookTitle() + "|" + getAuthor() + "|" + getGenre();
    }

    // DO NOT ADD SETTERS
}
