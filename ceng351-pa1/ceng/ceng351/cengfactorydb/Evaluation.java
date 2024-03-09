package ceng.ceng351.cengfactorydb;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.DecimalFormat;
import java.util.Vector;


public class Evaluation {

    private static String user = "e2644391"; // TODO: Your userName
    private static String password = "ZfoDZqEeQxw6"; //  TODO: Your password
    private static String host = "144.122.71.128"; // host name
    private static String database = "db2644391"; // TODO: Your database name
    private static int port = 8080; // port

    private static Connection connection = null;

    public static void connect() {

        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection =  DriverManager.getConnection(url, user, password);
        }
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    public static void disconnect() {

        if (connection != null) {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

    public static void addInputTitle(String title, BufferedWriter bufferedWriter) throws IOException {
        bufferedWriter.write("*** " + title + " ***" + System.getProperty("line.separator"));
    }

    private static final DecimalFormat df = new DecimalFormat("0.00");

    public static void printAllTables(BufferedWriter bufferedWriter) throws IOException {

        String sql1 = "show tables";
        String sql2 = "describe ";

        Vector<String> tables = new Vector<>();

        try
        {
            // Execute query
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(sql1);

            // Process the result set
            while(rs.next()) {
                tables.add(rs.getString(1));
            }

            for(int i=0; i < tables.size(); i++) {
                rs = st.executeQuery(sql2 + tables.get(i));

                // Print table name
                bufferedWriter.write("--- " + tables.get(i) + " ---" + System.getProperty("line.separator"));

                // Print field names and types
                while(rs.next()) {
                    bufferedWriter.write(rs.getString(1) + " " + rs.getString(2) + System.getProperty("line.separator"));
                }

                bufferedWriter.write(System.getProperty("line.separator"));
            }

        } catch (SQLException e) {
            printException(e);
        }
    }

    private static void printException(SQLException ex) {
        System.out.println(ex.getMessage() + "\n");
    }

    public static void printLine(String result, BufferedWriter bufferedWriter) throws IOException {
        bufferedWriter.write(result + System.getProperty("line.separator"));
    }

    public static void addDivider(BufferedWriter bufferedWriter) throws IOException {
        bufferedWriter.write( System.getProperty("line.separator")+ "--------------------------------------------------------------" + System.getProperty("line.separator"));
    }


    public static void main(String[] args) {

        int numberofInsertions = 0;
        int numberofTablesCreated = 0;
        int numberofTablesDropped = 0;

        /***********************************************************/
        // TODO While running on your local machine, change cengfactorydb accordingly
        String cengfactorydbDirectory = System.getProperty("user.dir") +
                "/ceng" + System.getProperty("file.separator") +
                "ceng351" + System.getProperty("file.separator") +
                "cengfactorydb";
        /***********************************************************/

        FileWriter fileWriter = null;
        BufferedWriter bufferedWriter = null;

        //Connect to the database
        connect();

        // Create CENGFACTORYDB object
        CENGFACTORYDB cengfactoryDB = null;

        String country = "Turkey";
        String productType = "Food";
        int factoryId = 100;
        String department = "Sales";
        String updateProductType = "Technology";
        double percentage = 0.3;
        String givenDate = "2020-01-01";


        try {
            // Create CENGFACTORYDB object and initialize
            cengfactoryDB = new CENGFACTORYDB();
            cengfactoryDB.initialize();

            /***********************************************************/
            /*************Create File Writer starts*********************/
            /***********************************************************/
            fileWriter = FileOperations.createFileWriter(cengfactorydbDirectory + System.getProperty("file.separator") + "output" + System.getProperty("file.separator") + "Output.txt");
            bufferedWriter =  new BufferedWriter(fileWriter);
            /***********************************************************/
            /*************Create File Writer ends***********************/
            /***********************************************************/

            /***********************************************************/
            /*************Drop tables starts****************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Drop tables", bufferedWriter);
            numberofTablesDropped = 0;

            // Drop tables
            try {
                numberofTablesDropped = cengfactoryDB.dropTables();
            } catch(Exception e) {
                e.printStackTrace();
            }

            // Check if tables are dropped
            printLine("Dropped " + numberofTablesDropped + " tables.", bufferedWriter);

            addDivider(bufferedWriter);
            /***********************************************************/
            /*************Drop tables ends******************************/
            /***********************************************************/

            /***********************************************************/
            /*******************Create tables starts********************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Create tables",bufferedWriter);
            numberofTablesCreated = 0;

            // Create Tables
            try {
                numberofTablesCreated = cengfactoryDB.createTables();

                // Check if tables are created
                printLine("Created " + numberofTablesCreated + " tables.", bufferedWriter);

            } catch(Exception e) {
                printLine("Q3.1: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }

            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Create tables ends**********************/
            /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO Factory starts**********/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Factory",bufferedWriter);
            //insert Factory
            numberofInsertions = 0;
            Factory[] factories = FileOperations.readFactoryFile(cengfactorydbDirectory +
                    System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                    "Test_FactoryFile.txt");
            for (Factory f:
                    factories) {
                System.out.println(f.toString());
            }

            numberofInsertions = cengfactoryDB.insertFactory(factories);
            printLine( numberofInsertions + " factories are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Factory ends************/
            /***********************************************************/


            /***********************************************************/
            /*******************Insert INTO Employee starts*************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Employee",bufferedWriter);
            //insert employee
            numberofInsertions = 0;
            Employee[] employees = FileOperations.readEmployeeFile(cengfactorydbDirectory +
                    System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                    "Test_EmployeeFile.txt");

            numberofInsertions = cengfactoryDB.insertEmployee(employees);
            printLine(numberofInsertions + " employees are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Employee ends***************/
            /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO WorksIn starts*****************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into WorksIn",bufferedWriter);
            //insert WorksIn
            numberofInsertions = 0;
            WorksIn[] worksIns = FileOperations.readWorksInFile( cengfactorydbDirectory + System.getProperty("file.separator")
                    + "data" + System.getProperty("file.separator") + "Test_WorksInFile.txt");

            numberofInsertions = cengfactoryDB.insertWorksIn(worksIns);
            printLine( numberofInsertions + " WorksIns are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO WorksIn ends*******************/
            /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO Product starts**********/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Product",bufferedWriter);
            //insert Product
            numberofInsertions = 0;
            Product[] products = FileOperations.readProductFile(cengfactorydbDirectory +
                    System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                    "Test_ProductFile.txt");

            numberofInsertions = cengfactoryDB.insertProduct(products);
            printLine( numberofInsertions + " products are inserted.", bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Product ends************/
            /***********************************************************/


            /***********************************************************/
            /*******************Insert INTO Produce starts*************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Produce",bufferedWriter);
            //insert Produce
            numberofInsertions = 0;
            Produce[] produces = FileOperations.readProduceFile(cengfactorydbDirectory +
                    System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                    "Test_ProduceFile.txt");

            numberofInsertions = cengfactoryDB.insertProduce(produces);
            printLine(numberofInsertions + " produces are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Produce ends***************/
            /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO Shipment starts*************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Shipment",bufferedWriter);
            //insert Shipment
            numberofInsertions = 0;
            Shipment[] shipments = FileOperations.readShipmentFile(cengfactorydbDirectory +
                    System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                    "Test_ShipmentFile.txt");

            numberofInsertions = cengfactoryDB.insertShipment(shipments);
            printLine(numberofInsertions + " shipments are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Shipment ends***************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Find all factories that are located in a particular country",bufferedWriter);
            try {

                Factory[] factories1 = cengfactoryDB.getFactoriesForGivenCountry(country);

                //Header Line
                printLine("Factories",bufferedWriter);

                if(factories1 != null) {
                    for(Factory factory : factories1){
                        printLine(factory.toString(),bufferedWriter);
                    }
                }


            } catch(Exception e) {
                printLine("Q3.3: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Find all factories without any working employees.",bufferedWriter);
            try {

                Factory[] factories1 = cengfactoryDB.getFactoriesWithoutAnyEmployee();

                //Header Line
                printLine("Factories",bufferedWriter);

                if(factories1 != null) {
                    for(Factory factory: factories1){
                        printLine(factory.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.4: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List all factories that produce all products for a particular productType",bufferedWriter);
            try {

                Factory[] factories1 = cengfactoryDB.getFactoriesProducingAllForGivenType(productType);

                //Header Line
                printLine("Factories",bufferedWriter);

                if(factories1 != null) {
                    for(Factory factory : factories1){
                        printLine(factory.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.5: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }

            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/


            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List the products that are produced in a particular factory but don’t have any shipment from that factory.",bufferedWriter);
            try {

                Product[] products1 = cengfactoryDB.getProductsProducedNotShipped();

                //Header Line
                printLine("Product",bufferedWriter);

                if(products1 != null) {
                    for(Product product : products1){
                        printLine(product.toString(),bufferedWriter);
                    }
                }
            } catch(Exception e) {
                printLine("Q3.6: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("For a given factoryId and department, list the average salary of the employees working in that factory and that specific department.",bufferedWriter);
            try {

                double avgSalary = cengfactoryDB.getAverageSalaryForFactoryDepartment(factoryId, department);
                //Header Line
                printLine("Average",bufferedWriter);

                printLine(String.valueOf(avgSalary),bufferedWriter);


            } catch(Exception e) {
                printLine("Q3.7: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List the most profitable products for each factory",bufferedWriter);
            try {

                QueryResult.MostValueableProduct[] mostValueableProducts = cengfactoryDB.getMostValueableProducts();

                //Header Line
                printLine("Most Valueable Products",bufferedWriter);

                if(mostValueableProducts != null) {
                    for(QueryResult.MostValueableProduct mostValueableProduct : mostValueableProducts){
                        printLine(mostValueableProduct.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.8: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List the most profit gaining factories for each product",bufferedWriter);
            try {

                QueryResult.MostValueableProduct[] mostValueableProducts = cengfactoryDB.getMostValueableProductsOnFactory();

                //Header Line
                printLine("Most Earning Factories for Product",bufferedWriter);

                if(mostValueableProducts != null) {
                    for(QueryResult.MostValueableProduct mostValueableProduct : mostValueableProducts){
                        printLine(mostValueableProduct.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.8: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle(" For each department, list all employees that are paid under the average salary for that department.",bufferedWriter);

            try {

                QueryResult.LowSalaryEmployees[] lowSalaryEmployees = cengfactoryDB.getLowSalaryEmployeesForDepartments();
                //Header Line
                printLine("Low Salary Employees", bufferedWriter);

                if (lowSalaryEmployees != null) {
                    for(QueryResult.LowSalaryEmployees lowSalaryEmployee : lowSalaryEmployees){
                        printLine(lowSalaryEmployee.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.9: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("For the products of given productType, increase the productionCost of every unit by a given percentage.",bufferedWriter);
            try {

                int numberofChanged = cengfactoryDB.increaseCost(updateProductType, percentage);
                printLine( numberofChanged + " rows are changed.", bufferedWriter);

            } catch(Exception e) {
                printLine("Q3.11: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Delete all employees that have not worked since the given date.",bufferedWriter);
            try {

                int numberofChanged = cengfactoryDB.deleteNotWorkingEmployees(givenDate);
                //Header Line
                printLine( numberofChanged + " rows are changed.", bufferedWriter);

            } catch(Exception e) {
                printLine("Q3.12: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                //Close Writer
                if (bufferedWriter != null) {
                    bufferedWriter.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            //Close Connection
            disconnect();
        }
    }

}
