package ceng.ceng351.cengfactorydb;

public class Shipment {

    private int factoryId;
    private int productId;
    private int amount;
    private int pricePerUnit;

    public Shipment(int factoryId, int productId, int amount, int pricePerUnit) {
        this.factoryId = factoryId;
        this.productId = productId;
        this.amount = amount;
        this.pricePerUnit = pricePerUnit;
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

    public int getAmount() {
        return amount;
    }

    public void setAmount(int amount) {
        this.amount = amount;
    }

    public int getPricePerUnit() {
        return pricePerUnit;
    }

    public void setPricePerUnit(int pricePerUnit) {
        this.pricePerUnit = pricePerUnit;
    }

    @Override
    public String toString() {
        return "Shipment{" +
                "factoryId=" + factoryId +
                ", productId=" + productId +
                ", amount=" + amount +
                ", pricePerUnit=" + pricePerUnit +
                '}';
    }
}
