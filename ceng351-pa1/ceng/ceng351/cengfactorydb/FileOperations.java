package ceng.ceng351.cengfactorydb;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;


public class FileOperations {

    public static FileWriter createFileWriter( String path) throws IOException {
        File f = new File( path);

        FileWriter fileWriter = null;

        if( f.isDirectory() && !f.exists())
            f.mkdirs();
        else if( !f.isDirectory() && !f.getParentFile().exists())
            f.getParentFile().mkdirs();

        if( !f.isDirectory() && f.exists())
            f.delete();

        fileWriter = new FileWriter( f, false);

        return fileWriter;
    }



    public static Factory[] readFactoryFile(String filePath){
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;

        String strLine;

        List<Factory> factoryList = new ArrayList<>();
        Factory[] factories = null;

        try {

            fileReader = new FileReader(filePath);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //factoryId factoryName    factoryType    country

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");

                if (words.length < 4) {
                    System.out.println("There is a problem in Factory File Reading phase");
                }
                else {
                    int factoryId = Integer.parseInt(words[0]);
                    String factoryName = words[1];
                    String factoryType = words[2];
                    String country = words[3];

                    Factory factory = new Factory(factoryId, factoryName, factoryType, country);
                    factoryList.add(factory);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            factories = new Factory[factoryList.size()];
            factoryList.toArray(factories);

        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return factories;
    }

    public static Employee[] readEmployeeFile(String filePath){
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;

        String strLine;

        List<Employee> employeeList = new ArrayList<>();
        Employee[] employees = null;

        try {

            fileReader = new FileReader(filePath);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //employeeId employeeName    department    salary

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");

                if (words.length < 4) {
                    System.out.println("There is a problem in Employee File Reading phase");
                }
                else {
                    int employeeId = Integer.parseInt(words[0]);
                    String employeeName = words[1];
                    String department = words[2];
                    int salary = Integer.parseInt(words[3]);

                    Employee employee = new Employee(employeeId, employeeName, department, salary);
                    employeeList.add(employee);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            employees = new Employee[employeeList.size()];
            employeeList.toArray(employees);

        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return employees;
    }

    public static WorksIn[] readWorksInFile(String filePath){
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;

        String strLine;

        List<WorksIn> worksInList = new ArrayList<>();
        WorksIn[] worksIns = null;

        try {

            fileReader = new FileReader(filePath);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //factoryId employeeId    startDate

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");

                if (words.length < 3) {
                    System.out.println("There is a problem in WorksIn File Reading phase");
                }
                else {
                    int factoryId = Integer.parseInt(words[0]);
                    int employeeId = Integer.parseInt(words[1]);
                    String startDate = words[2];


                    WorksIn worksIn = new WorksIn(employeeId, factoryId, startDate);
                    worksInList.add(worksIn);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            worksIns = new WorksIn[worksInList.size()];
            worksInList.toArray(worksIns);

        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return worksIns;
    }

    public static Product[] readProductFile(String filePath){
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;

        String strLine;

        List<Product> productList = new ArrayList<>();
        Product[] products = null;

        try {

            fileReader = new FileReader(filePath);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //productId productName productType

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");

                if (words.length < 3) {
                    System.out.println("There is a problem in Product File Reading phase");
                }
                else {
                    int productId = Integer.parseInt(words[0]);
                    String productName = words[1];
                    String productType = words[2];

                    Product product = new Product(productId, productName, productType);
                    productList.add(product);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            products = new Product[productList.size()];
            productList.toArray(products);

        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return products;
    }

    public static Produce[] readProduceFile(String filePath){
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;

        String strLine;

        List<Produce> produceList = new ArrayList<>();
        Produce[] produces = null;

        try {

            fileReader = new FileReader(filePath);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //factoryId productId    amount    productionCost

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");

                if (words.length < 4) {
                    System.out.println("There is a problem in Produce File Reading phase");
                }
                else {
                    int factoryId = Integer.parseInt(words[0]);
                    int productId = Integer.parseInt(words[1]);
                    int amount = Integer.parseInt(words[2]);
                    int productionCost = Integer.parseInt(words[3]);

                    Produce produce = new Produce(factoryId, productId, amount, productionCost);
                    produceList.add(produce);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            produces = new Produce[produceList.size()];
            produceList.toArray(produces);

        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return produces;
    }

    public static Shipment[] readShipmentFile(String filePath){
        FileReader fileReader = null;
        BufferedReader bufferedReader = null;

        String strLine;

        List<Shipment> shipmentList = new ArrayList<>();
        Shipment[] shipments = null;

        try {

            fileReader = new FileReader(filePath);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //factoryId productId    amount    pricePerUnit

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");

                if (words.length < 4) {
                    System.out.println("There is a problem in Shipment File Reading phase");
                }
                else {
                    int factoryId = Integer.parseInt(words[0]);
                    int productId = Integer.parseInt(words[1]);
                    int amount = Integer.parseInt(words[2]);
                    int pricePerUnit = Integer.parseInt(words[3]);

                    Shipment shipment = new Shipment(factoryId, productId, amount, pricePerUnit);
                    shipmentList.add(shipment);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            shipments = new Shipment[shipmentList.size()];
            shipmentList.toArray(shipments);

        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return shipments;
    }
}
