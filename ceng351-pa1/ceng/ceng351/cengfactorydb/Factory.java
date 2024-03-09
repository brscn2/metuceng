package ceng.ceng351.cengfactorydb;

public class Factory {

    private int factoryId;
    private String factoryName;
    private String factoryType;
    private String country;

    public Factory(int factoryId, String factoryName, String factoryType, String country) {
        this.factoryId = factoryId;
        this.factoryName = factoryName;
        this.factoryType = factoryType;
        this.country = country;
    }

    public int getFactoryId() {
        return factoryId;
    }

    public void setFactoryId(int factoryId) {
        this.factoryId = factoryId;
    }

    public String getFactoryName() {
        return factoryName;
    }

    public void setFactoryName(String factoryName) {
        this.factoryName = factoryName;
    }

    public String getFactoryType() {
        return factoryType;
    }

    public void setFactoryType(String factoryType) {
        this.factoryType = factoryType;
    }

    public String getCountry() {
        return country;
    }

    public void setCountry(String country) {
        this.country = country;
    }

    @Override
    public String toString() {
        return "Factory{" +
                "factoryId=" + factoryId +
                ", factoryName='" + factoryName + '\'' +
                ", factoryType='" + factoryType + '\'' +
                ", country='" + country + '\'' +
                '}';
    }
}