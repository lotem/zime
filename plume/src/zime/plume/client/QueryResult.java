package zime.plume.client;

import java.io.Serializable;

public class QueryResult extends Query implements Serializable {

    private static final long serialVersionUID = -6333820525948383847L;

    private boolean ok;
    private boolean error;
    private int invalidPos;

    public QueryResult(Query query, String text) {
        super(query.schemaName, query.input);
        // TODO
        ok = false;
        error = false;
        invalidPos = 0;
    }
    
    @Override
    public String toString() {
        String state = ok ? "OK" : error ? "ERROR" : "PENDING"; 
        return "<QueryResult: " + schemaName + "[" + input + "] state=" + state + " invalidPos=" + invalidPos + ">";
    }
    
    @Override
    public boolean equals(Object obj) {
        if (obj instanceof QueryResult) {
            return input.equals(((QueryResult) obj).input);
        }
        return false;
    }
    
    public void markOK() {
        ok = true;
    }
    
    public boolean isOK() {
        return ok;
    }

    public void markError() {
        error = true;
    }
    
    public boolean hasError() {
        return error;
    }
    
    public int getInvalidPos() {
        return invalidPos;
    }
    
    public void setInvalidPos(int invalidPos) {
        this.invalidPos = invalidPos;
    }

}
