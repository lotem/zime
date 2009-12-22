package zime.plume.client;

public class Query {

    protected String schemaName;
    protected String input;

    public Query(String schemaName, String input) {
        this.schemaName = schemaName;
        this.input = input;
    }

    public Query() {
    }

    @Override
    public String toString() {
        return "<Query: " + schemaName + "[" + input + "]>";
    }
    
    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Query) {
            return input.equals(((Query) obj).input);
        }
        return false;
    }
    
    public String getSchemaName() {
        return schemaName;
    }

    public String getInput() {
        return input;
    }

}
