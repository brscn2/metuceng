package ceng.ceng351.cengfactorydb;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class CENGFACTORYDB implements ICENGFACTORYDB {
    private Connection conn;
    private final String user = "e2644391";
    private final String password = "ZfoDZqEeQxw6";
    /*
     * private String database = "db2644391";
     * private String host = "144.122.71.128";
     * private String port = "8080";
     */

    /**
     * Place your initialization code inside if required.
     *
     * <p>
     * This function will be called before all other operations. If your
     * implementation
     * need initialization , necessary operations should be done inside this
     * function. For
     * example, you can set your connection to the database server inside this
     * function.
     */
    public void initialize() {
        /* Register the driver with the DriverManager */
        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
        String connectionString = "jdbc:mysql://144.122.71.128:8080/db2644391?useSSL=false";

        try {
            conn = DriverManager.getConnection(connectionString, user, password);
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        dropTables();
    }

    /**
     * Should create the necessary tables when called.
     *
     * @return the number of tables that are created successfully.
     */
    public int createTables() {
        int numberOfTablesCreated = 0;
        try {
            Statement stmt = conn.createStatement();

            String factorySchema = "CREATE TABLE Factory (\n" +
                    "  factoryId int primary key,\n" +
                    "  factoryName text,\n" +
                    "  factoryType text,\n" +
                    "  country text\n" +
                    ");";
            stmt.executeUpdate(factorySchema);
            numberOfTablesCreated++;

            String employeeSchema = "CREATE TABLE Employee (\n" +
                    "  employeeId int primary key,\n" +
                    "  employeeName text,\n" +
                    "  department text,\n" +
                    "  salary int\n" +
                    ");";
            stmt.executeUpdate(employeeSchema);
            numberOfTablesCreated++;

            String worksInSchema = "CREATE TABLE `Works_In` (\n" +
                    "  factoryId int,\n" +
                    "  employeeId int,\n" +
                    "  startDate date,\n" +
                    "  primary key (factoryId, employeeId),\n" +
                    "  foreign key (factoryId) references Factory(factoryId) on delete cascade,\n" +
                    "  foreign key (employeeId) references Employee(employeeId) on delete cascade\n" +
                    ");";
            stmt.executeUpdate(worksInSchema);
            numberOfTablesCreated++;

            String productSchema = "CREATE TABLE Product (\n" +
                    "  productId int primary key,\n" +
                    "  productName text,\n" +
                    "  productType text\n" +
                    ");\n";
            stmt.executeUpdate(productSchema);
            numberOfTablesCreated++;

            String produceSchema = "CREATE TABLE Produce (\n" +
                    "  factoryId int,\n" +
                    "  productId int,\n" +
                    "  amount int,\n" +
                    "  productionCost int,\n" +
                    "  primary key (factoryId, productId),\n" +
                    "  foreign key (factoryId) references Factory(factoryId),\n" +
                    "  foreign key (productId) references Product(productId)\n" +
                    ");\n";
            stmt.executeUpdate(produceSchema);
            numberOfTablesCreated++;

            String shipmentSchema = "CREATE TABLE Shipment (\n" +
                    "  factoryId int,\n" +
                    "  productId int,\n" +
                    "  amount int,\n" +
                    "  pricePerUnit int,\n" +
                    "  primary key (factoryId, productId),\n" +
                    "  foreign key (factoryId) references Factory(factoryId),\n" +
                    "  foreign key (productId) references Product(productId)\n" +
                    ");\n";
            stmt.executeUpdate(shipmentSchema);
            numberOfTablesCreated++;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfTablesCreated;
    }

    /**
     * Should drop the tables if exists when called.
     *
     * @return the number of tables are dropped successfully.
     */
    public int dropTables() {
        int dropCount = 0;
        String[] tableNames = { "Shipment", "Produce", "Product", "Works_In", "Employee", "Factory" };

        for (String tableName : tableNames) {
            try {
                Statement stmt = conn.createStatement();
                stmt.executeUpdate("DROP TABLE IF EXISTS `" + tableName + "`;");
                dropCount++;
                stmt.close();
            } catch (SQLException ex) {
                ex.printStackTrace();
            }

        }


        return dropCount;
    }

    /**
     * Should insert an array of Factory into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertFactory(Factory[] factories) {
        int numberOfRowsInserted = 0;
        try {
            String factoryInsertQuery = "INSERT INTO Factory(factoryId, factoryName, factoryType, country) VALUES (?, ?, ?, ?)";
            PreparedStatement preparedStatement = conn.prepareStatement(factoryInsertQuery);

            int tempNumberOfRows = 0;
            for (Factory factory : factories) {
                preparedStatement.setInt(1, factory.getFactoryId());
                preparedStatement.setString(2, factory.getFactoryName());
                preparedStatement.setString(3, factory.getFactoryType());
                preparedStatement.setString(4, factory.getCountry());
                preparedStatement.addBatch();
                tempNumberOfRows++;
            }

            preparedStatement.executeBatch();
            numberOfRowsInserted = tempNumberOfRows;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfRowsInserted;
    }

    /**
     * Should insert an array of Employee into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertEmployee(Employee[] employees) {
        int numberOfRowsInserted = 0;
        try {
            String employeeInsertQuery = "INSERT INTO Employee(employeeId, employeeName, department, salary) VALUES (?, ?, ?, ?)";
            PreparedStatement preparedStatement = conn.prepareStatement(employeeInsertQuery);

            int tempNumberOfRows = 0;
            for (Employee employee : employees) {
                preparedStatement.setInt(1, employee.getEmployeeId());
                preparedStatement.setString(2, employee.getEmployeeName());
                preparedStatement.setString(3, employee.getDepartment());
                preparedStatement.setInt(4, employee.getSalary());
                preparedStatement.addBatch();
                tempNumberOfRows++;
            }

            preparedStatement.executeBatch();
            numberOfRowsInserted = tempNumberOfRows;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfRowsInserted;
    }

    /**
     * Should insert an array of WorksIn into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertWorksIn(WorksIn[] worksIns) {
        int numberOfRowsInserted = 0;
        try {
            String worksInInsertQuery = "INSERT INTO Works_In(factoryId, employeeId, startDate) VALUES (?, ?, ?)";
            PreparedStatement preparedStatement = conn.prepareStatement(worksInInsertQuery);

            int tempNumberOfRows = 0;
            for (WorksIn worksIn : worksIns) {
                preparedStatement.setInt(1, worksIn.getFactoryId());
                preparedStatement.setInt(2, worksIn.getEmployeeId());
                preparedStatement.setDate(3, Date.valueOf(worksIn.getStartDate()));
                preparedStatement.addBatch();
                tempNumberOfRows++;
            }

            preparedStatement.executeBatch();
            numberOfRowsInserted = tempNumberOfRows;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfRowsInserted;
    }

    /**
     * Should insert an array of Product into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduct(Product[] products) {
        int numberOfRowsInserted = 0;

        try {
            String productInsertQuery = "INSERT INTO Product(productId, productName, productType) VALUES (?, ?, ?)";
            PreparedStatement preparedStatement = conn.prepareStatement(productInsertQuery);

            int tempNumberOfRows = 0;
            for (Product product : products) {
                preparedStatement.setInt(1, product.getProductId());
                preparedStatement.setString(2, product.getProductName());
                preparedStatement.setString(3, product.getProductType());
                preparedStatement.addBatch();
                tempNumberOfRows++;
            }

            preparedStatement.executeBatch();
            numberOfRowsInserted = tempNumberOfRows;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfRowsInserted;
    }

    /**
     * Should insert an array of Produce into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduce(Produce[] produces) {
        int numberOfRowsInserted = 0;

        try {
            String produceInsertQuery = "INSERT INTO Produce(factoryId, productId, amount, productionCost) VALUES (?, ?, ?, ?)";
            PreparedStatement preparedStatement = conn.prepareStatement(produceInsertQuery);

            int tempNumberOfRows = 0;
            for (Produce produce : produces) {
                preparedStatement.setInt(1, produce.getFactoryId());
                preparedStatement.setInt(2, produce.getProductId());
                preparedStatement.setInt(3, produce.getAmount());
                preparedStatement.setInt(4, produce.getProductionCost());
                preparedStatement.addBatch();
                tempNumberOfRows++;
            }

            preparedStatement.executeBatch();
            numberOfRowsInserted = tempNumberOfRows;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfRowsInserted;
    }

    /**
     * Should insert an array of Shipment into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertShipment(Shipment[] shipments) {
        int numberOfRowsInserted = 0;

        try {
            String shipmentInsertQuery = "INSERT INTO Shipment(factoryId, productId, amount, pricePerUnit) VALUES (?, ?, ?, ?)";
            PreparedStatement preparedStatement = conn.prepareStatement(shipmentInsertQuery);

            int tempNumberOfRows = 0;
            for (Shipment shipment : shipments) {
                preparedStatement.setInt(1, shipment.getFactoryId());
                preparedStatement.setInt(2, shipment.getProductId());
                preparedStatement.setInt(3, shipment.getAmount());
                preparedStatement.setInt(4, shipment.getPricePerUnit());
                preparedStatement.addBatch();
                tempNumberOfRows++;
            }

            preparedStatement.executeBatch();
            numberOfRowsInserted = tempNumberOfRows;
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return numberOfRowsInserted;
    }

    /**
     * Should return all factories that are located in a particular country.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesForGivenCountry(String country) {
        List<Factory> factoryList = new ArrayList<Factory>();
        try {
            String getFactoryByCountryQuery = "SELECT DISTINCT * FROM Factory F WHERE F.country = ? ORDER BY factoryId ASC";
            PreparedStatement stmt = conn.prepareStatement(getFactoryByCountryQuery);
            stmt.setString(1, country);
            ResultSet factories = stmt.executeQuery();
            while (factories.next()) {
                int factoryId = factories.getInt(1);
                String factoryName = factories.getString(2);
                String factoryType = factories.getString(3);
                String rsCountry = factories.getString(4);
                factoryList.add(new Factory(factoryId, factoryName, factoryType, rsCountry));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        Factory[] factoryArray = new Factory[factoryList.size()];
        factoryList.toArray(factoryArray);

        return factoryArray;
    }

    /**
     * Should return all factories without any working employees.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesWithoutAnyEmployee() {
        List<Factory> factoryList = new ArrayList<Factory>();

        String getFactoriesWithoutAnyEmployeeQuery = "SELECT DISTINCT * FROM Factory F WHERE NOT EXISTS (SELECT * FROM Works_In W WHERE F.factoryId = W.factoryId) ORDER BY factoryId ASC";
        try {
            Statement stmt = conn.createStatement();
            ResultSet factories = stmt.executeQuery(getFactoriesWithoutAnyEmployeeQuery);

            while (factories.next()) {
                int factoryId = factories.getInt(1);
                String factoryName = factories.getString(2);
                String factoryType = factories.getString(3);
                String rsCountry = factories.getString(4);
                factoryList.add(new Factory(factoryId, factoryName, factoryType, rsCountry));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        Factory[] factoryArray = new Factory[factoryList.size()];
        factoryList.toArray(factoryArray);

        return factoryArray;
    }

    /**
     * Should return all factories that produce all products for a particular
     * productType
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesProducingAllForGivenType(String productType) {
        List<Factory> factoryList = new ArrayList<Factory>();

        String getFactoriesProducingAllForGivenTypeQuery = "SELECT DISTINCT * FROM Factory F WHERE NOT EXISTS (SELECT P.productId FROM Product P WHERE P.productType = ? EXCEPT SELECT P2.productId FROM Produce P2 WHERE P2.factoryId = F.factoryId) ORDER BY factoryId ASC";
        try {
            PreparedStatement stmt = conn.prepareStatement(getFactoriesProducingAllForGivenTypeQuery);
            stmt.setString(1, productType);
            ResultSet factories = stmt.executeQuery();

            while (factories.next()) {
                int factoryId = factories.getInt(1);
                String factoryName = factories.getString(2);
                String factoryType = factories.getString(3);
                String rsCountry = factories.getString(4);
                factoryList.add(new Factory(factoryId, factoryName, factoryType, rsCountry));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        Factory[] factoryArray = new Factory[factoryList.size()];
        factoryList.toArray(factoryArray);

        return factoryArray;
    }

    /**
     * Should return the products that are produced in a particular factory but
     * don’t have any shipment from that factory.
     *
     * @return Product[]
     */
    public Product[] getProductsProducedNotShipped() {
        List<Product> productList = new ArrayList<Product>();

        String getProductsProducedNotShippedQuery = "SELECT DISTINCT * FROM Product P WHERE P.productId IN (SELECT DISTINCT P1.productId FROM Produce P1 WHERE P1.productId = P.productId AND NOT EXISTS (SELECT DISTINCT S.productId FROM Shipment S WHERE S.factoryId = P1.factoryId AND S.productId = P1.productId))";
        try {
            Statement stmt = conn.createStatement();
            ResultSet products = stmt.executeQuery(getProductsProducedNotShippedQuery);

            while (products.next()) {
                int productId = products.getInt(1);
                String productName = products.getString(2);
                String productType = products.getString(3);
                productList.add(new Product(productId, productName, productType));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        Product[] productArray = new Product[productList.size()];
        productList.toArray(productArray);

        return productArray;
    }

    /**
     * For a given factoryId and department, should return the average salary of
     * the employees working in that factory and that specific department.
     *
     * @return double
     */
    public double getAverageSalaryForFactoryDepartment(int factoryId, String department) {
        double avgSalary = 0.0;
        String getAverageSalaryForFactoryDepartment = "SELECT DISTINCT AVG(E.salary) FROM Employee E, Works_In W WHERE E.employeeId = W.employeeId AND W.factoryId = ? AND E.department = ?";
        try {
            PreparedStatement stmt = conn.prepareStatement(getAverageSalaryForFactoryDepartment);
            stmt.setInt(1, factoryId);
            stmt.setString(2, department);
            ResultSet rsAvg = stmt.executeQuery();

            while (rsAvg.next()) {
                avgSalary = rsAvg.getDouble(1);
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }
        return avgSalary;
    }

    /**
     * Should return the most profitable products for each factory
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProducts() {
        List<QueryResult.MostValueableProduct> mvpList = new ArrayList<>();

        String getMostValuableProductsQuery = "SELECT DISTINCT S.factoryId, S.productId, P1.productName, P1.productType, ((S.amount * S.pricePerUnit) - (P.amount * P.productionCost)) AS profit FROM Produce P, Shipment S, Product P1 WHERE P.factoryId = S.factoryId AND P.productId = S.productId AND P1.productId = P.productId AND ((S.amount * S.pricePerUnit) - (P.amount * P.productionCost)) >= (SELECT MAX((S1.amount * S1.pricePerUnit) - (P1.amount * P1.productionCost)) FROM Produce P1, Shipment S1 WHERE P1.factoryId = S1.factoryId AND P1.productId = S1.productId AND P1.factoryId = S.factoryId) ORDER BY profit DESC";
        try {
            Statement stmt = conn.createStatement();
            ResultSet mostValuableProducts = stmt.executeQuery(getMostValuableProductsQuery);

            while (mostValuableProducts.next()) {
                int factoryId = mostValuableProducts.getInt(1);
                int productId = mostValuableProducts.getInt(2);
                String productName = mostValuableProducts.getString(3);
                String productType = mostValuableProducts.getString(4);
                double profit = mostValuableProducts.getDouble(5);

                mvpList.add(
                        new QueryResult.MostValueableProduct(factoryId, productId, productName, productType, profit));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        QueryResult.MostValueableProduct[] mvpArray = new QueryResult.MostValueableProduct[mvpList.size()];
        mvpList.toArray(mvpArray);

        return mvpArray;
    }

    /**
     * For each product, return the factories that gather the highest profit
     * for that product
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProductsOnFactory() {
        List<QueryResult.MostValueableProduct> mvpList = new ArrayList<>();

        String getMostValuableProductsOnFactoryQuery = "SELECT DISTINCT S.factoryId, S.productId, P.productName, P.productType, ((S.amount * S.pricePerUnit) - (P1.amount * P1.productionCost)) AS profit\n"
                +
                "FROM Product P, Produce P1, Shipment S\n" +
                "WHERE P.productId = P1.productId AND \n" +
                "      P1.factoryId = S.factoryId AND\n" +
                "      P1.productId = S.productId AND\n" +
                "      ((S.amount * S.pricePerUnit) - (P1.amount * P1.productionCost)) >= \n" +
                "          (\n" +
                "            SELECT MAX((S1.amount * S1.pricePerUnit) - (P2.amount * P2.productionCost))\n" +
                "            FROM Produce P2, Shipment S1\n" +
                "            WHERE P2.productId = S1.productId AND\n" +
                "                  P2.factoryId = S1.factoryId AND\n" +
                "                  P2.productId = P.productId\n" +
                "          )\n" +
                "ORDER BY profit DESC";
        try {
            Statement stmt = conn.createStatement();
            ResultSet mostValuableProducts = stmt.executeQuery(getMostValuableProductsOnFactoryQuery);

            while (mostValuableProducts.next()) {
                int factoryId = mostValuableProducts.getInt(1);
                int productId = mostValuableProducts.getInt(2);
                String productName = mostValuableProducts.getString(3);
                String productType = mostValuableProducts.getString(4);
                double profit = mostValuableProducts.getDouble(5);

                mvpList.add(
                        new QueryResult.MostValueableProduct(factoryId, productId, productName, productType, profit));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        QueryResult.MostValueableProduct[] mvpArray = new QueryResult.MostValueableProduct[mvpList.size()];
        mvpList.toArray(mvpArray);

        return mvpArray;
    }

    /**
     * For each department, should return all employees that are paid under the
     * average salary for that department. You consider the employees
     * that do not work earning ”0”.
     *
     * @return QueryResult.LowSalaryEmployees[]
     */
    public QueryResult.LowSalaryEmployees[] getLowSalaryEmployeesForDepartments() {
        List<QueryResult.LowSalaryEmployees> lowSalaryEmployeesList = new ArrayList<>();

        String getLowSalaryEmployeesForDepartmentsQuery = "SELECT DISTINCT employeeId, employeeName, department, salary\n" +
                "FROM Employee E\n" +
                "WHERE E.salary < (SELECT AVG(salary) FROM Employee E1 WHERE E1.department = E.department)\n" +
                "ORDER BY employeeId ASC";

        try {
            Statement stmt = conn.createStatement();
            ResultSet lowSalaryEmployees = stmt.executeQuery(getLowSalaryEmployeesForDepartmentsQuery);

            while (lowSalaryEmployees.next()) {
                int employeeId = lowSalaryEmployees.getInt(1);
                String employeeName = lowSalaryEmployees.getString(2);
                String department = lowSalaryEmployees.getString(3);
                int salary = lowSalaryEmployees.getInt(4);

                lowSalaryEmployeesList
                        .add(new QueryResult.LowSalaryEmployees(employeeId, employeeName, department, salary));
            }
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        QueryResult.LowSalaryEmployees[] lowSalaryEmployeesArray = new QueryResult.LowSalaryEmployees[lowSalaryEmployeesList
                .size()];
        lowSalaryEmployeesList.toArray(lowSalaryEmployeesArray);

        return lowSalaryEmployeesArray;
    }

    /**
     * For the products of given productType, increase the productionCost of every
     * unit by a given percentage.
     *
     * @return number of rows affected
     */
    public int increaseCost(String productType, double percentage) {
        String increaseCostQuery = "UPDATE Produce\n" +
                "SET productionCost = productionCost * ((100 + ?) / 100)\n" +
                "WHERE Produce.productId IN (SELECT P.productId FROM Product P WHERE P.productType = ?)";

        int affectedRowCount = 0;

        try {
            PreparedStatement stmt = conn.prepareStatement(increaseCostQuery);
            stmt.setDouble(1, percentage);
            stmt.setString(2, productType);
            affectedRowCount = stmt.executeUpdate();
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return affectedRowCount;
    }

    /**
     * Deleting all employees that have not worked since the given date.
     *
     * @return number of rows affected
     */
    public int deleteNotWorkingEmployees(String givenDate) {
        String deleteNotWorkingEmployeesQuery = "DELETE FROM Employee\n" +
                "WHERE NOT EXISTS (SELECT E1.employeeId \n" +
                "                  FROM Employee E1, Works_In W \n" +
                "                  WHERE E1.employeeId = W.employeeId AND\n" +
                "                        E1.employeeId = Employee.employeeId AND\n" +
                "                        W.startDate > ?)";

        int affectedRowCount = 0;

        try {
            PreparedStatement stmt = conn.prepareStatement(deleteNotWorkingEmployeesQuery);
            stmt.setDate(1, Date.valueOf(givenDate));
            affectedRowCount = stmt.executeUpdate();
        } catch (SQLException ex) {
            ex.printStackTrace();
        }

        return affectedRowCount;
    }

    /**
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * THE METHODS AFTER THIS LINE WILL NOT BE GRADED.
     * YOU DON'T HAVE TO SOLVE THEM, LEAVE THEM AS IS IF YOU WANT.
     * IF YOU HAVE ANY QUESTIONS, REACH ME VIA EMAIL.
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     */

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Peers are considered tied and receive the same rank. After
     * that, there should be a gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Everything is the same but after ties, there should be no
     * gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank2() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each factory, calculate the most profitable 4th product.
     *
     * @return QueryResult.FactoryProfit
     */
    public QueryResult.FactoryProfit calculateFourth() {
        return new QueryResult.FactoryProfit(0, 0, 0);
    }

    /**
     * Determine the salary variance between an employee and another
     * one who began working immediately after the first employee (by
     * startDate), for all employees.
     *
     * @return QueryResult.SalaryVariant[]
     */
    public QueryResult.SalaryVariant[] calculateVariance() {
        return new QueryResult.SalaryVariant[0];
    }

    /**
     * Create a method that is called once and whenever a Product starts
     * losing money, deletes it from Produce table
     *
     * @return void
     */
    public void deleteLosing() {

    }
}