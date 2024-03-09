import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

public class CengTreeParser {
    public static ArrayList<CengBook> parseBooksFromFile(String filename) {
        ArrayList<CengBook> bookList = new ArrayList<>();

        // You need to parse the input file in order to use GUI tables.
        // TODO: Parse the input file, and convert them into CengBooks
        try (Scanner sc = new Scanner(new FileInputStream(filename))) {
            while (sc.hasNextLine()) {
                String[] splitBook = sc.nextLine().split("\\|");
                for (String s : splitBook) {
                    System.out.println(s);
                }
                CengBook newBook = new CengBook(Integer.parseInt(splitBook[0]), splitBook[1], splitBook[2],
                        splitBook[3]);
                bookList.add(newBook);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return bookList;
    }

    public static void startParsingCommandLine() throws IOException {
        // TODO: Start listening and parsing command line -System.in-.
        // There are 4 commands:
        // 1) quit : End the app, gracefully. Print nothing, call nothing, just break
        // off your command line loop.
        // 2) add : Parse and create the book, and call
        // CengBookRunner.addBook(newlyCreatedBook).
        // 3) search : Parse the bookID, and call CengBookRunner.searchBook(bookID).
        // 4) print : Print the whole tree, call CengBookRunner.printTree().

        // Commands (quit, add, search, print) are case-insensitive.

        Scanner sc = new Scanner(System.in);
        while (true) {
            String[] splitCommand = sc.nextLine().split("\\|");

            if (splitCommand[0].equals("quit"))
                break;

            String command = splitCommand[0];
            switch (command) {
                case "add" -> {
                    CengBook newBook = new CengBook(Integer.parseInt(splitCommand[1]), splitCommand[2], splitCommand[3],
                            splitCommand[4]);
                    CengBookRunner.addBook(newBook);
                }
                case "print" -> CengBookRunner.printTree();
                case "search" -> {
                    Integer searchKey = Integer.parseInt(splitCommand[1]);
                    CengBookRunner.searchBook(searchKey);
                }
            }
        }
        sc.close();
    }
}
