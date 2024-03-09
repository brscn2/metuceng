package ceng.ceng351.cengfactorydb;

public class WorksIn {

    private int employeeId;
    private int factoryId;
    private String startDate;

    public WorksIn(int employeeId, int factoryId, String startDate) {
        this.employeeId = employeeId;
        this.factoryId = factoryId;
        this.startDate = startDate;
    }

    public int getEmployeeId() {
        return employeeId;
    }

    public void setEmployeeId(int employeeId) {
        this.employeeId = employeeId;
    }

    public int getFactoryId() {
        return factoryId;
    }

    public void setFactoryId(int factoryId) {
        this.factoryId = factoryId;
    }

    public String getStartDate() {
        return startDate;
    }

    public void setStartDate(String startDate) {
        this.startDate = startDate;
    }

    @Override
    public String toString() {
        return "WorksIn{" +
                "employeeId=" + employeeId +
                ", factoryId=" + factoryId +
                ", startDate='" + startDate + '\'' +
                '}';
    }
}
