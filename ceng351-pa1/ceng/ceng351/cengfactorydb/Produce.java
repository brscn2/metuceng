package ceng.ceng351.cengfactorydb;

public class Produce {

    private int factoryId;
    private int productId;
    private int amount;
    private int productionCost;

    public Produce(int factoryId, int productId, int amount, int productionCost) {
        this.factoryId = factoryId;
        this.productId = productId;
        this.amount = amount;
        this.productionCost = productionCost;
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

    public int getProductionCost() {
        return productionCost;
    }

    public void setProductionCost(int productionCost) {
        this.productionCost = productionCost;
    }

    @Override
    public String toString() {
        return "Produce{" +
                "factoryId=" + factoryId +
                ", productId=" + productId +
                ", amount=" + amount +
                ", productionCost=" + productionCost +
                '}';
    }
}