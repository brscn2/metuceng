package ceng.ceng351.cengfactorydb;

public class QueryResult {

    public static class MostValueableProduct {
        private int factoryId;
        private int productId;
        private String productName;
        private String productType;
        private double profit;

        public MostValueableProduct(int factoryId, int productId, String productName, String productType, double profit) {
            this.factoryId = factoryId;
            this.productId = productId;
            this.productName = productName;
            this.productType = productType;
            this.profit = profit;
        }

        public int getFactoryId() {
            return factoryId;
        }

        public void setFactoryId(int factoryId) {
            this.factoryId = factoryId;
        }

        public int getProductId() {
            return productId;
        }

        public void setProductId(int productId) {
            this.productId = productId;
        }

        public String getProductName() {
            return productName;
        }

        public void setProductName(String productName) {
            this.productName = productName;
        }

        public String getProductType() {
            return productType;
        }

        public void setProductType(String productType) {
            this.productType = productType;
        }

        public double getProfit() {
            return profit;
        }

        public void setProfit(double profit) {
            this.profit = profit;
        }

        @Override
        public String toString() {
            return "MostValueableProduct{" +
                    "factoryId=" + factoryId +
                    ", productId=" + productId +
                    ", productName='" + productName + '\'' +
                    ", productType='" + productType + '\'' +
                    ", profit=" + profit +
                    '}';
        }
    }

    public static class LowSalaryEmployees {
        private int employeeId;
        private String employeeName;
        private String department;
        private int salary;

        public LowSalaryEmployees(int employeeId, String employeeName, String department, int salary) {
            this.employeeId = employeeId;
            this.employeeName = employeeName;
            this.department = department;
            this.salary = salary;
        }

        public int getEmployeeId() {
            return employeeId;
        }

        public void setEmployeeId(int employeeId) {
            this.employeeId = employeeId;
        }

        public String getEmployeeName() {
            return employeeName;
        }

        public void setEmployeeName(String employeeName) {
            this.employeeName = employeeName;
        }

        public String getDepartment() {
            return department;
        }

        public void setDepartment(String department) {
            this.department = department;
        }

        public int getSalary() {
            return salary;
        }

        public void setSalary(int salary) {
            this.salary = salary;
        }

        @Override
        public String toString() {
            return "LowSalaryEmployees{" +
                    "employeeId=" + employeeId +
                    ", employeeName='" + employeeName + '\'' +
                    ", department='" + department + '\'' +
                    ", salary=" + salary +
                    '}';
        }
    }

    public static class EmployeeRank {
        private int employeeId;
        private String department;
        private int salary;
        private int rank;

        public EmployeeRank(int employeeId, String department, int salary, int rank) {
            this.employeeId = employeeId;
            this.department = department;
            this.salary = salary;
            this.rank = rank;
        }

        public int getEmployeeId() {
            return employeeId;
        }

        public void setEmployeeId(int employeeId) {
            this.employeeId = employeeId;
        }

        public String getDepartment() {
            return department;
        }

        public void setDepartment(String department) {
            this.department = department;
        }

        public int getSalary() {
            return salary;
        }

        public void setSalary(int salary) {
            this.salary = salary;
        }

        public int getRank() {
            return rank;
        }

        public void setRank(int rank) {
            this.rank = rank;
        }

        @Override
        public String toString() {
            return "EmployeeRank{" +
                    "employeeId=" + employeeId +
                    ", department='" + department + '\'' +
                    ", salary=" + salary +
                    ", rank=" + rank +
                    '}';
        }
    }

    public static class FactoryProfit{
        private int factoryId;
        private int productId;
        private int profit;

        public FactoryProfit(int factoryId, int productId, int profit) {
            this.factoryId = factoryId;
            this.productId = productId;
            this.profit = profit;
        }

        public int getFactoryId() {
            return factoryId;
        }

        public void setFactoryId(int factoryId) {
            this.factoryId = factoryId;
        }

        public int getProductId() {
            return productId;
        }

        public void setProductId(int productId) {
            this.productId = productId;
        }

        public int getProfit() {
            return profit;
        }

        public void setProfit(int profit) {
            this.profit = profit;
        }

        @Override
        public String toString() {
            return "FactoryProfit{" +
                    "factoryId=" + factoryId +
                    ", productId=" + productId +
                    ", profit=" + profit +
                    '}';
        }
    }

    public static class SalaryVariant{
        private int employeeId;
        private String startDate;
        private int salary;
        private int salaryDifference;

        public SalaryVariant(int employeeId, String startDate, int salary, int salaryDifference) {
            this.employeeId = employeeId;
            this.startDate = startDate;
            this.salary = salary;
            this.salaryDifference = salaryDifference;
        }

        public int getEmployeeId() {
            return employeeId;
        }

        public void setEmployeeId(int employeeId) {
            this.employeeId = employeeId;
        }

        public String getStartDate() {
            return startDate;
        }

        public void setStartDate(String startDate) {
            this.startDate = startDate;
        }

        public int getSalary() {
            return salary;
        }

        public void setSalary(int salary) {
            this.salary = salary;
        }

        public int getSalaryDifference() {
            return salaryDifference;
        }

        public void setSalaryDifference(int salaryDifference) {
            this.salaryDifference = salaryDifference;
        }

        @Override
        public String toString() {
            return "SalaryVariant{" +
                    "employeeId=" + employeeId +
                    ", startDate='" + startDate + '\'' +
                    ", salary=" + salary +
                    ", salaryDifference=" + salaryDifference +
                    '}';
        }
    }
}
