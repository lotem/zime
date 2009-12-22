package zime.plume.client;

import java.util.List;


public interface Backend {
    
    public interface LoadSchemaListCallback {
        void onReady(List<Schema> config);
        void onFailure();
    }
    
    public interface LoadConfigCallback {
        void onReady(Config config);
        void onFailure();
    }
    
    public interface QueryCallback {
        void onReady(QueryResult result);
        void onFailure();
    }
    
    void loadSchemaList(LoadSchemaListCallback callback);
    void loadConfig(String schema, LoadConfigCallback callback);
    void doQuery(Query query, QueryCallback callback);
    
}
