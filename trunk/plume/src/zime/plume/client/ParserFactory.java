package zime.plume.client;

public interface ParserFactory {

    public String getName();

    public Parser create(Schema schema, KeyEventHandler fallback);

}
