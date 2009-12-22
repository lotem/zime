package zime.plume.client;

import zime.plume.client.util.Logger;

public class Context {

    private static final int EDIT_MODE = 0;
    private static final int CONVERSION_MODE = 1;
    private static final int ERROR_MODE = 2;
    
    private Schema schema;
    private Engine engine;
    private StringBuffer input;
    private QueryResult queryResult;
    private int mode;
    private int startPos;
    private int endPos;

    public Context(Schema schema, Engine engine) {
        this.schema = schema;
        this.engine = engine;
        input = new StringBuffer();
        queryResult = null;
        edit();
    }

    public boolean isEmpty() {
        return input.length() == 0;
    }
    
    public boolean isEditMode() {
        return mode == EDIT_MODE;
    }
    
    public boolean isConversionMode() {
        return mode == CONVERSION_MODE;
    }
    
    public boolean isErrorMode() {
        return mode == ERROR_MODE;
    }
    
    public int getStartPos() {
        return startPos;
    }
    
    public int getEndPos() {
        return endPos;
    }
    
    public void startConversion(Backend backend) {
        Logger.debug("startConversion: ");
        String input = getInputString();
        if (queryResult != null) {
            if (input.equals(queryResult.getInput())) {
                // reuse previous query result
                if (queryResult.isOK()) {
                    nextConversion();
                } 
                else if (queryResult.hasError()) {
                    markError(queryResult.getInvalidPos());
                } 
                else {
                    // pending; do nothing
                }
                return;
            }
        }
        backend.doQuery(new Query(schema.getName(), input), new Backend.QueryCallback() {
            @Override
            public void onFailure() {
                // TODO
            }
            @Override
            public void onReady(QueryResult result) {
                Logger.debug("QueryCallback.onReady: " + result);
                if (!result.equals(queryResult))
                    return;
                queryResult = result;
                if (result.isOK()) {
                    nextConversion();
                }
                else if (result.hasError()) {
                    markError(result.getInvalidPos());
                }
            }
        });
    }

    public void nextConversion() {
        Logger.debug("nextConversion: ");
        // TODO
        convert(0, 0);
    }
    
    public void previousConversion() {
        Logger.debug("previousConversion: ");
        // TODO
        edit();
    }
    
    private void convert(int start, int end) {
        mode = CONVERSION_MODE;
        startPos = start;
        endPos = end;
    }
    
    private void edit() {
        mode = EDIT_MODE;
        startPos = 0;
        endPos = 0;
    }
    
    public void markError(int start) {
        Logger.debug("markError: " + start);
        mode = ERROR_MODE;
        startPos = start;
        endPos = getInputLength();
        update();
    }
    
    public void clearError() {
        input.setLength(startPos);
        edit();
        update();
    }
    
    public String getPreedit() {
        // TODO
        return input.toString();
    }

    public CandidateList getCandidateList() {
        // TODO Auto-generated method stub
        return new CandidateList(null);
    }

    private void update() {
        engine.onContextUpdate();
    }

    public void clear() {
        input.setLength(0);
        edit();
        update();
    }

    public String getInputString() {
        return input.toString();
    }

    public int getInputLength() {
        return input.length();
    }

    public void truncateInput(int length) {
        input.setLength(length);
        edit();
        update();
    }
    
    public void appendInput(String s) {
        input.append(s);
        edit();
        update();
    }

}
