package cn.zzsst.client;

public abstract class QueryCallback implements Callback {
    
    protected Query query;
    protected Callback callback;

    public QueryCallback(Query query, Callback callback) {
        this.query = query;
        this.callback = callback;
    }

}
